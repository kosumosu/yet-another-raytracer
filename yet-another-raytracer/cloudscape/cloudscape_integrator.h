#pragma once

#include "atmosphere_math.h"

#include "color/color_functions.hpp"
#include "math/random.hpp"

#include "prepared_scene.h"

#include "integrators/RayIntegrator.h"
#include "math/ray.hpp"

#include <PerlinNoise.hpp>

#include <array>
#include <limits>
#include <variant>

#include "participating_media/DynamicCompositeMedium.h"
#include "participating_media/HomogeneousMedium.h"

namespace cloudscape
{
    struct media_interaction_none
    {
        color_rgb transmittance;
    };

    struct media_interaction_absorbtion
    {
        color_rgb emission;
    };

    struct media_interaction_scatter
    {
        ray3 new_ray;
        color_rgb transmittance;
        participating_media::phase_function_t phase_function; // in case we want to evaluate other lighting
    };

    using media_trace_result = std::variant<media_interaction_none, media_interaction_absorbtion,
                                            media_interaction_scatter>;


    struct atmospheric_sample
    {
        color_rgb inscatter;
        color_rgb transparency;
    };

    template <Marcher_c TMarcher>
    class CloudscapeIntegrator final : public RayIntegrator
    {
    public:
        using raytracer_t = Raytracer<TMarcher>;

    private:
        constexpr static space_real BIAS = std::numeric_limits<space_real>::epsilon() * space_real(16.0);
        constexpr static space_real EPS = std::numeric_limits<space_real>::epsilon();
        constexpr static float MAX_DIST = std::numeric_limits<space_real>::max();

        const prepared_scene scene_;
        raytracer_t raytracer_;
        const participating_media::ParticipatingMedium& atmosphericMedium_;
        const participating_media::ParticipatingMedium& cloudsMedium_;

        const DirectionalLightSource& sun_;

        const siv::PerlinNoise noise_; //TODO each thread has it's own noise

        participating_media::DynamicCompositeMedium media_;
        participating_media::DynamicCompositeMedium shadowMedia_;

    public:
        CloudscapeIntegrator(
            prepared_scene scene,
            raytracer_t raytracer
        )
            : scene_(std::move(scene))
              , raytracer_{std::move(raytracer)}
              , atmosphericMedium_(*scene_.atmospheric_medium)
              , cloudsMedium_(*scene_.cloud_medium)
              , sun_(scene_.sun)
        {
        }

        color_rgb EvaluateRay(const ray3& ray, space_real bias,
                              math::Sampler<space_real>& sampler) override
        {
            return EvaluateRayNew(ray, bias, sampler);

            //return EvaluateRayImpl(ray, bias, sampler, 2);
        }

        color_rgb EvaluateRayNew(const ray3& ray, space_real bias,
                                 math::Sampler<space_real>& sampler) noexcept
        {
            const auto lower_cloud_bound2 = square(scene_.lower_cloud_bound.radius());
            const auto upper_cloud_bound2 = square(scene_.upper_cloud_bound.radius());

            const auto dist_to_planet_center2 = math::length2(ray.origin() - scene_.planet.center());
            const auto is_inside_clouds = dist_to_planet_center2 > lower_cloud_bound2 && dist_to_planet_center2 <
                upper_cloud_bound2;

            color_rgb integral(color_rgb::zero()); // sort of sub integral or just single sample
            color_rgb throughput(color_rgb::fill(1));

            media_.ClearMedia();
            media_.PushMedium(&atmosphericMedium_);
            if (is_inside_clouds)
            {
                media_.PushMedium(&cloudsMedium_);
            }

            auto currentRay = ray;
            bool accountForEmission = true;
            bool earlyExit = false;

            auto hit = raytracer_.TraceRay(ray, bias);

            std::size_t depth = 0;
            for (; !earlyExit; ++depth)
            {
                const auto obstruction_free_distance = hit.has_occurred()
                                                           ? hit.distance()
                                                           : std::numeric_limits<space_real>::max();

                auto media_result = TraceMedia(media_, currentRay, obstruction_free_distance, sampler);

                if (const auto absorption = std::get_if<media_interaction_absorbtion>(&media_result))
                {
                    integral += absorption->emission * throughput;
                    break;
                }
                else if (const auto scatter = std::get_if<media_interaction_scatter>(&media_result))
                {
                    throughput *= scatter->transmittance;
                    //throughput *= color_real(0.5); // because we add up two sources

                    sun_.DoWithDistribution(
                        scatter->new_ray.origin(),
                        sampler,
                        [&](const light_distribution& lightDistribution)
                        {
                            if (lightDistribution.delta_components() != 0 || lightDistribution.
                                has_non_delta_component())
                            {
                                const auto optionalLightSample = lightDistribution.generate_sample();

                                if (optionalLightSample.getValue())
                                {
                                    const auto& lightSample = *optionalLightSample.getValue();

                                    const ray3 light_ray{scatter->new_ray.origin(), lightSample.direction};

                                    const auto light_ray_transmittance = EvaluateTransmittanceAlongSegment(
                                        light_ray,
                                        lightSample.distance - BIAS * lightSample.distance,
                                        sampler
                                    );

                                    if (!math::isZero(light_ray_transmittance))
                                    {
                                        auto lightValue = lightSample.evaluate();

                                        const auto scattering_coeffs = scatter->phase_function(
                                            currentRay.direction(), lightSample.direction);

                                        const auto lightSamplePdf = color_real(optionalLightSample.getPdf());

                                        auto radianceByLight =
                                            lightValue
                                            * scattering_coeffs
                                            * color_real(math::oneOverPi)
                                            / lightSamplePdf;

                                        integral += radianceByLight * light_ray_transmittance * throughput;
                                    }
                                }
                            }
                        });

                    const auto kill_probability = std::clamp(color::get_importance(scatter->transmittance),
                                                             color_real(0.25), color_real(0.99));

                    // Termination by russian roulette
                    // a workaround since uniform_random_generator occasionally generates 1.0f when it should not.
                    if (kill_probability < color_real(1.0) && sampler.Get1D() >= kill_probability || kill_probability ==
                        color_real(
                            0.0))
                    {
                        break;
                    }
                    else
                    {
                        throughput = throughput / kill_probability;
                    }

                    currentRay = scatter->new_ray;

                    continue;
                }

                const auto no_interaction = std::get<media_interaction_none>(media_result);
                throughput *= no_interaction.transmittance;

                if (!hit.has_occurred())
                    break;

                hit.object()->material()->WithBsdfDistribution(
                    *hit.object(),
                    hit.point(),
                    hit.normal(),
                    hit.uvs(),
                    currentRay.direction(),
                    sampler,
                    [&](const materials::bsdf_distribution& bsdfDistribution)
                    {
                        const auto bsdfSample = bsdfDistribution.generate_sample();
                        const auto bsdfDirection = bsdfSample.getValue().outgoingDirection;
                        const auto bsdfColor = bsdfSample.getValue().evaluate();
                        const auto bsdfPdf = color_real(bsdfSample.getPdf());

                        const bool entering = math::is_obtuse_angle(currentRay.direction(), hit.normal());
                        color_rgb radianceAtCurrentPathVertex(color_rgb::zero());

                        if (bsdfDistribution.has_non_delta_component())
                            radianceAtCurrentPathVertex += EvaluateRadianceByLightsAtVertex(
                                currentRay,
                                hit,
                                sampler);

                        if (accountForEmission)
                            radianceAtCurrentPathVertex += hit.object()->material()->EvaluateEmission(
                                *hit.object(),
                                hit.point(),
                                hit.normal(),
                                hit.uvs(),
                                currentRay.direction(),
                                sampler);

                        const auto samplePayload = radianceAtCurrentPathVertex * throughput;
                        assert(!math::anyNan(samplePayload));

                        integral += samplePayload;

                        assert(math::max_element(integral) < 20000.0f);

                        if (!bsdfDistribution.has_non_delta_component() && bsdfDistribution.delta_components() == 0)
                        {
                            earlyExit = true;
                            return;
                        }


                        const auto geometricTerm = color_real(std::abs(math::dot(bsdfDirection, hit.normal())));

                        const auto vertexThroughput = bsdfSample.getIsDelta()
                                                          ? bsdfColor * geometricTerm / bsdfPdf
                                                          : bsdfColor * geometricTerm * color_real(math::oneOverPi) /
                                                          bsdfPdf;

                        const auto importance = std::max(color_real(0.75), color::get_importance(vertexThroughput));

                        // Termination by russian roulette
                        // a workaround since uniform_random_generator occasionally generates 1.0f when it should not.
                        if (importance < color_real(1.0) && sampler.Get1D() >= importance || importance == color_real(
                            0.0))
                        {
                            earlyExit = true;
                            return;
                        }
                        else
                        {
                            throughput *= vertexThroughput / std::min(color_real(1.0), importance);
                            assert(!math::anyNan(throughput));
                            assert(math::max_element(throughput) < 100000.0f);
                        }

                        if (bsdfSample.getValue().isTransition && hit.object()->inner_medium())
                        {
                            if (entering)
                            {
                                assert(
                                    dynamic_cast<const participating_media::VoidMedium*>(hit.object()->inner_medium())
                                    == nullptr);
                                media_.PushMedium(hit.object()->inner_medium());
                            }
                            else
                            {
                                media_.PopMedium(hit.object()->inner_medium());
                            }
                        }

                        currentRay = ray3(hit.point(), bsdfDirection);
                        accountForEmission = bsdfSample.getIsDelta();

                        const auto rt_bias = BiasForPosition(currentRay.origin());
                        const auto new_hit = raytracer_.TraceRay(currentRay, rt_bias,
                                                                 std::numeric_limits<space_real>::max(),
                                                                 hit.object(),
                                                                 hit.normal());
                        if (hit.point() == new_hit.point())
                        {
                            int sdf = 0;
                        }
                        hit = std::move(new_hit);
                    });
            }

            return integral;
        }

        media_trace_result TraceMedia(participating_media::DynamicCompositeMedium& medium, const ray3& ray,
                                      space_real max_distance, math::Sampler<space_real>& sampler)
        {
            auto traversed_distance = space_real(0);
            auto majorant_transmittance = color_rgb::one();

            auto current_ray = ray;
            while (true)
            {
                const auto majorant_extinction = medium.SampleMajorantExtinction(current_ray, max_distance);

                const auto majorant_sampling_extinction = ExtinctionForSamping(majorant_extinction);

                if (majorant_sampling_extinction == color_real(0))
                {
                    return media_interaction_none{
                        majorant_transmittance
                    };
                }

                const auto distance =
                    math::sampleExponential(color_real(sampler.Get1D()), majorant_sampling_extinction);
                //const auto distance_pdf = math::exponentialPDF(distance, majorant_sampling_extinction);

                if (traversed_distance + distance > max_distance)
                {
                    const auto last_piece_extinction = math::exp(
                        -majorant_extinction * color_real(max_distance - traversed_distance));
                    majorant_transmittance *= last_piece_extinction;
                    return media_interaction_none{
                        majorant_transmittance
                    };
                }

                const auto piece_transmittance = math::exp(-majorant_extinction * distance);
                majorant_transmittance *= piece_transmittance;
                traversed_distance += distance;

                const auto new_ray = current_ray.ray_along(distance);
                auto media_properties = medium.SampleProperties(new_ray.origin());

                const auto absorption_prob = ExtinctionForSamping(media_properties.absorption) /
                    majorant_sampling_extinction;
                const auto scattering_prob = ExtinctionForSamping(media_properties.scattering) /
                    majorant_sampling_extinction;

                const auto random_value = sampler.Get1D();
                if (random_value <= absorption_prob)
                {
                    return media_interaction_absorbtion{
                        majorant_transmittance * media_properties.emission
                    };
                }
                else if (random_value <= absorption_prob + scattering_prob)
                {
                    // todo do scattering
                    const auto scattering_sample = media_properties.scatter_generator(
                        sampler, [](const auto& extinction) { return ExtinctionForSamping(extinction); });

                    // todo: sometimes we want to scatter towards sun

                    return media_interaction_scatter{
                        {new_ray.origin(), scattering_sample.direction},
                        scattering_sample.transmittance,
                        std::move(media_properties.phase_function)
                    };
                }
                else
                {
                    // null scattering

                    // Russian roulette
                    const auto importance = std::max(color_real(0.1), color::get_importance(piece_transmittance));
                    // a workaround since uniform_random_generator occasionally generates 1.0f when it should not.
                    if (importance < color_real(1.0) && sampler.Get1D() >= importance || importance == color_real(0.0))
                    {
                        return media_interaction_absorbtion{
                            color_rgb::zero()
                        };
                    }
                    else
                    {
                        majorant_transmittance *= piece_transmittance / std::min(color_real(1.0), importance);
                        assert(!math::anyNan(majorant_transmittance));
                        assert(math::max_element(majorant_transmittance) < 100000.0f);
                    }

                    current_ray = new_ray;
                }
            }
        }


        // TODO get rid of this
        statistics::Stats getStats() const
        {
            return {};
        }

    private:
        [[nodiscard]] static space_real BiasForPosition(const vector3& position)
        {
            return std::max(space_real(std::numeric_limits<float>::epsilon()),
                            std::abs(math::max_element(position)) * EPS * space_real(8192.0));
        }

        [[nodiscard]] static color_real ExtinctionForSamping(const color_rgb& extinction)
        {
            return extinction[1]; // Just take green for now
        }

        color_rgb EvaluateTransmittanceAlongSegment(
            const ray3& ray,
            const space_real& distance,
            math::Sampler<space_real>& sampler)
        {
            shadowMedia_.CopyMedia(media_);

            color_rgb throughput(color_rgb::fill(1));

            auto currentRay = ray;
            auto distance_left = distance;

            const auto rt_bias = BiasForPosition(currentRay.origin());
            auto hit = raytracer_.TraceRay(ray, rt_bias, distance_left, nullptr, -ray.direction());
            // wtf is directionToIgnore ?

            while (distance_left > space_real(0))
            {
                const bool entering = math::is_obtuse_angle(currentRay.direction(), hit.normal());

                const auto obstruction_free_distance = hit.has_occurred()
                                                           ? hit.distance()
                                                           : std::numeric_limits<space_real>::max();

                const auto limited_obstruction_free_distance = std::min(distance_left, hit.distance());
                distance_left -= limited_obstruction_free_distance;

                auto media_result = TraceMedia(shadowMedia_, currentRay, limited_obstruction_free_distance, sampler);

                if (const auto absorption = std::get_if<media_interaction_absorbtion>(&media_result))
                {
                    throughput = color_rgb::zero();
                    break;
                }
                else if (const auto scatter = std::get_if<media_interaction_scatter>(&media_result))
                {
                    throughput = color_rgb::zero();
                    break;
                }

                const auto no_interaction = std::get<media_interaction_none>(media_result);
                throughput *= no_interaction.transmittance;

                if (!hit.has_occurred())
                    break;

                const auto surface_transmittance = hit.object()->material()->EvaluateTransmittance(
                    *hit.object(),
                    hit.point(),
                    hit.normal(),
                    hit.uvs(),
                    currentRay.direction(),
                    sampler
                );

                const auto importance = std::max(color_real(0.1), color::get_importance(surface_transmittance));

                if (math::isZero(surface_transmittance) || importance < color_real(1.0) && sampler.Get1D() >= importance
                    || importance == color_real(
                        0.0))
                {
                    throughput = color_rgb::zero();
                    break;
                }
                else
                {
                    throughput *= surface_transmittance / std::min(color_real(1.0), importance);
                    assert(!math::anyNan(throughput));
                    assert(math::max_element(throughput) < 100000.0f);
                }

                if (entering)
                {
                    shadowMedia_.PushMedium(hit.object()->inner_medium());
                }
                else
                {
                    shadowMedia_.PopMedium(hit.object()->inner_medium());
                }

                currentRay = ray3(hit.point(), currentRay.direction());
                const auto rt_bias = BiasForPosition(currentRay.origin());
                hit = raytracer_.TraceRay(currentRay, rt_bias, std::numeric_limits<space_real>::max(),
                                          hit.object(),
                                          hit.normal());
            }

            return throughput;
        }

        color_rgb EvaluateRadianceByLightsAtVertex(
            const ray3& currentRay,
            const Hit& hit,
            math::Sampler<space_real>& sampler)
        {
            color_rgb radianceAtCurrentPathVertex{color_rgb::zero()};
            const LightingContext context{hit.object(), hit.point(), hit.normal(), BIAS, 1, false};

            const auto& light = sun_;
            light.DoWithDistribution(
                context,
                sampler,
                [&](const light_distribution& lightDistribution)
                {
                    if (lightDistribution.delta_components() != 0 || lightDistribution.has_non_delta_component())
                    {
                        const auto optionalLightSample = lightDistribution.generate_sample();

                        if (optionalLightSample.getValue())
                        {
                            const auto& lightSample = *optionalLightSample.getValue();

                            const auto brdfValue = hit.object()->material()->
                                                       EvaluateNonDeltaScattering(
                                                           *hit.object(),
                                                           hit.point(),
                                                           hit.normal(),
                                                           hit.uvs(),
                                                           currentRay.direction(),
                                                           lightSample.direction,
                                                           sampler);

                            if (!math::isZero(brdfValue))
                            {
                                const auto light_ray_transmittance = EvaluateTransmittanceAlongSegment(
                                    {hit.point(), lightSample.direction},
                                    lightSample.distance - BIAS * lightSample.distance,
                                    sampler
                                );

                                if (!math::isZero(light_ray_transmittance))
                                {
                                    auto geometricTerm = color_real(
                                        std::abs(math::dot(lightSample.direction, hit.normal())));

                                    auto lightValue = lightSample.evaluate();


                                    const auto lightSamplePdf = color_real(optionalLightSample.getPdf());

                                    auto radianceByLight = lightValue * brdfValue
                                        * geometricTerm * color_real(math::oneOverPi)
                                        / lightSamplePdf;

                                    //if (radianceByLight[0] < color_real(0.0) || radianceByLight[1] < color_real(0.0) || radianceByLight[2] < color_real(0.0))
                                    //	throw std::exception();

                                    radianceAtCurrentPathVertex = radianceByLight * light_ray_transmittance;

                                    assert(math::max_element(radianceAtCurrentPathVertex) < 20000.0f);
                                }
                            }
                        }
                    }
                });

            return radianceAtCurrentPathVertex;
        }


        // result[3] is density, not transparency!
        [[nodiscard]] space_real SampleCloudVolume(const vector3& position) const
        {
            const auto scaled_pos = position / scene_.scene.noise.size;
            const auto shift = vector3{
                noise_.octave3D(scaled_pos[0], scaled_pos[1], scaled_pos[2], 3),
                noise_.octave3D(scaled_pos[0], scaled_pos[1], scaled_pos[2] + 5489484.f, 3),
                noise_.octave3D(scaled_pos[0], scaled_pos[1], scaled_pos[2] + 435489484.f, 3)
            };
            const auto final_pos = scaled_pos + shift * space_real(0.25);

            //return scene_.scene.clouds.fog * std::max(space_real(0.0), space_real(noise_.normalizedOctave3D(scaled_pos[0], scaled_pos[1], scaled_pos[2], 2)) - scene_.scene.clouds.coverage);

            return
                scene_.scene.clouds.fog
                * std::max(space_real(0.0),
                           space_real(
                               space_real(scene_.scene.clouds.coverage - 0.5) * space_real(2.0) + noise_.
                               normalizedOctave3D(final_pos[0], final_pos[1], final_pos[2],
                                                  int(scene_.scene.noise.detail),
                                                  scene_.scene.noise.multiplier)));
        }

        // should include sun
        [[nodiscard]] atmospheric_sample EvaluateAtmosphere(const ray3& ray) const
        {
            constexpr auto sun_angular_size_cos = std::cos(math::deg_to_rad(1.0));
            const auto sun_cos = math::dot(ray.direction(), scene_.sun_direction);
            const auto sun_addition = sun_cos >= sun_angular_size_cos ? scene_.sun_color : color_rgb::zero();
            return {
                color_rgb(0.5, 0.55, 0.8) + sun_addition,
                color_rgb::zero()
            };

            // auto beta_rayleigh = scene_.beta_total_rayleigh;
            // auto beta_mie = scene_.beta_total_mie;
            //
            //
        }

        [[nodiscard]] atmospheric_sample EvaluateAtmosphere(const vector3& start, const vector3& end) const
        {
            auto transparency = math::exp(-color_rgb(0.00010, 0.00015, 0.0002) * color_real(math::length(end - start)));
            return {
                color_rgb(0.5, 0.55, 0.8) * (color_rgb::fill(1.0) - transparency),
                std::move(transparency)
            };

            // auto beta_rayleigh = scene_.beta_total_rayleigh;
            // auto beta_mie = scene_.beta_total_mie;
            //
            //
        }
    };
}
