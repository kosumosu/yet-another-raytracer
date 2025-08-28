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

#include "lights/SunLightSource.h"
#include "participating_media/DynamicCompositeMedium.h"
#include "participating_media/HomogeneousMedium.h"

namespace cloudscape
{
    struct media_interaction_none
    {
        color_rgb transmittance;
    };

    struct media_interaction_absorption // left for termination by russian roulette
    {
        color_rgb emission;
    };

    struct media_interaction_scatter
    {
        ray3 new_ray;
        color_rgb transmittance_to_event;
        color_rgb scatter_transmittance;
        color_real scatter_pdf;
        participating_media::phase_function_t phase_function; // in case we want to evaluate other lighting
        participating_media::pdf_evaluator_t evaluate_pdf; // in case we want to evaluate other lighting
    };

    using media_trace_result = std::variant<media_interaction_none, media_interaction_absorption,
                                            media_interaction_scatter>;

    struct light_sample
    {
        color_rgb value = color_rgb::zero();
        vector3 direction = vector3::zero();
        color_real pdf = color_real(0);
        color_real scatter_dir_pdf = color_real(0);
    };

    template <Marcher_c TMarcher>
    class CloudscapeIntegrator final : public RayIntegrator
    {
    public:
        using raytracer_t = Raytracer<TMarcher>;

    private:
        constexpr static auto BIAS = std::numeric_limits<space_real>::epsilon() * space_real(16.0);
        constexpr static auto EPS = std::numeric_limits<space_real>::epsilon();
        constexpr static auto MAX_DIST = std::numeric_limits<space_real>::max();

        constexpr static auto MIN_SURVIVE_PROBABILITY = color_real(0.1);
        constexpr static auto MAX_SURVIVE_PROBABILITY = color_real(0.9999);

        const prepared_scene scene_;
        raytracer_t raytracer_;
        const participating_media::ParticipatingMedium& atmosphericMedium_;
        const participating_media::ParticipatingMedium& atmosphericAerosolMedium_;
        const participating_media::ParticipatingMedium& hazeMedium_;
        const participating_media::ParticipatingMedium& cloudsMedium_;

        const lights::SunLightSource& sun_;
        const lights::LightSource& light_source_;

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
              , atmosphericMedium_(*scene_.atmospheric_molecular_medium)
              , atmosphericAerosolMedium_(*scene_.atmospheric_aerosol_medium)
              , hazeMedium_(*scene_.haze_medium)
              , cloudsMedium_(*scene_.cloud_medium)
              , sun_(scene_.sun)
              , light_source_(scene_.directional_sun)
        {
        }

        color_rgb EvaluateRay(const ray3& ray, space_real bias,
                              math::Sampler<space_real>& sampler) override
        {
            return EvaluateRayNew(ray, bias, sampler);

            //return EvaluateRayImpl(ray, bias, sampler, 2);
        }

        void catch_shit(const color_rgb& color, size_t color_index)
        {
            const bool check =
                (color[0] == 0 && color_index != 0)
                && (color[1] == 0 && color_index != 1)
                && (color[2] == 0 && color_index != 2);
            if (check)
            {
                int x = 389;
            }
        }

        constexpr static void catch_invalid(const color_rgb& color)
        {
            if constexpr (false) {
                if (color[1] > 1)
                {
                    int x  = 0;
                }

                const bool invalid =
                    math::anyInvalid(color)
                || math::max_element(color) > 100000.0
                || math::min_element(color) < 0.0;
                if (invalid)
                {
                    int sd = 0;
                    throw std::logic_error("invalid color");
                }
                assert(!invalid);
            } else {
                // NOOP
            }
        }


        color_rgb EvaluateRayNew(const ray3& ray, space_real bias,
                                 math::Sampler<space_real>& sampler) noexcept
        {
            const auto lower_cloud_bound2 = square(scene_.lower_cloud_bound.radius());
            const auto upper_cloud_bound2 = square(scene_.upper_cloud_bound.radius());

            const auto dist_to_planet_center2 = math::length2(ray.origin() - scene_.planet.center());
            const auto is_inside_clouds = dist_to_planet_center2 > lower_cloud_bound2 && dist_to_planet_center2 <
                upper_cloud_bound2;


            const auto color_index = static_cast<size_t>(3 * sampler.Get1D());
            const auto color_pdf = color_real(1) / 3;


            // const auto random_var = std::min(color_real(0.99999), color_real(sampler.Get1D())); // to avoid getting 1 when casting to color_real. Sum of weights may also be less than 1, probably.
            // const auto color_index = color::sample_color_component(random_var);
            // const auto color_pdf = color::LUMINOCITY_WEIGHTS[color_index];


            color_rgb integral(color_rgb::zero()); // sort of sub integral or just single sample
            color_rgb throughput(color_rgb::zero());
            throughput[color_index] = 1 / color_pdf;

            media_.ClearMedia();
            media_.PushMedium(&atmosphericMedium_);
            media_.PushMedium(&atmosphericAerosolMedium_);
            // media_.PushMedium(&hazeMedium_);

            if (is_inside_clouds)
            {
                media_.PushMedium(&cloudsMedium_);
            }

            auto currentRay = ray;
            bool accountForEmission = true;
            color_real next_emission_weight = 1.0;
            bool earlyExit = false;

            auto hit = raytracer_.TraceRay(ray, bias);

            std::size_t depth = 0;
            for (; !earlyExit; ++depth)
            {
                const auto obstruction_free_distance = hit.has_occurred()
                                                           ? hit.distance()
                                                           : std::numeric_limits<space_real>::max();

                auto media_result = TraceMedia(media_, currentRay, obstruction_free_distance, throughput, color_index, sampler);

                if (const auto absorption = std::get_if<media_interaction_absorption>(&media_result))
                {
                    integral += throughput * absorption->emission;
                    catch_invalid(integral);
                    break;
                }
                else if (const auto scatter = std::get_if<media_interaction_scatter>(&media_result))
                {
                    throughput *= scatter->transmittance_to_event;
                    catch_invalid(throughput);
                    catch_invalid(integral);

                    auto light_sample = SampleLight(*scatter, currentRay, color_index, sampler);

                    if constexpr (false) // if using MIS
                    {
                        if (light_sample.pdf != color_real(0))
                        {
                            const auto light_dir_pdf_of_scatter = scatter->evaluate_pdf(
                                currentRay.direction(), light_sample.direction, color_index);
                            const auto light_pdf_sqr_sum = math::square(light_sample.pdf) + math::square(
                                light_dir_pdf_of_scatter);
                            const auto direct_light_weight = math::square(light_sample.pdf) / light_pdf_sqr_sum;
                            const auto direct_light = (light_sample.value / light_sample.pdf) * direct_light_weight;

                            integral += throughput * direct_light;
                            catch_invalid(integral);

                            const auto scatter_dir_pdf_of_light = light_sample.scatter_dir_pdf;
                            const auto scatter_pdf_sqr_sum = math::square(scatter->scatter_pdf) + math::square(
                                light_sample.scatter_dir_pdf);
                            next_emission_weight = math::square(scatter->scatter_pdf) / scatter_pdf_sqr_sum;
                            accountForEmission = true;
                        }
                        else
                        {
                            next_emission_weight = 1.0;
                        }
                    }
                    else
                    {
                        if (!math::isZero(light_sample.value))
                        {
                            integral += throughput * light_sample.value / light_sample.pdf;
                            catch_invalid(integral);
                        }
                        accountForEmission = false;
                    }

                    throughput *= scatter->scatter_transmittance / scatter->scatter_pdf;
                    catch_invalid(throughput);

                    const auto total_scatter_transmittance = scatter->transmittance_to_event * scatter->
                        scatter_transmittance / scatter->scatter_pdf;
                    const auto survive_probability = std::clamp(color::get_survive_importance(total_scatter_transmittance),
                                                                MIN_SURVIVE_PROBABILITY, MAX_SURVIVE_PROBABILITY);

                    // Termination by russian roulette
                    // a workaround since uniform_random_generator occasionally generates 1.0f when it should not.
                    if (sampler.Get1D() >= survive_probability)
                    {
                        break;
                    }
                    else
                    {
                        throughput = throughput / survive_probability;
                    }

                    currentRay = scatter->new_ray;

                    const auto rt_bias = BiasForPosition(currentRay.origin());
                    hit = raytracer_.TraceRay(currentRay, rt_bias,
                                              std::numeric_limits<space_real>::max(),
                                              hit.object(),
                                              hit.normal());
                    continue;
                }

                const auto no_interaction = std::get<media_interaction_none>(media_result);
                auto prev_thr = throughput;
                throughput *= no_interaction.transmittance;
                catch_invalid(throughput);

                if (!hit.has_occurred())
                {
                    integral += EvaluateInfinity(currentRay, accountForEmission * next_emission_weight) * throughput;
                    catch_invalid(integral);
                    break;
                }

                hit.object()->material()->WithBsdfDistribution(
                    *hit.object(),
                    hit.point(),
                    hit.normal(),
                    hit.uvs(),
                    currentRay.direction(),
                    sampler,
                    [&](const materials::bsdf_distribution& bsdfDistribution)
                    {
                        color_rgb radianceAtCurrentPathVertex(color_rgb::zero());

                        if (accountForEmission)
                            radianceAtCurrentPathVertex += hit.object()->material()->EvaluateEmission(
                                *hit.object(),
                                hit.point(),
                                hit.normal(),
                                hit.uvs(),
                                currentRay.direction(),
                                sampler);

                        if (!bsdfDistribution.has_non_delta_component() && bsdfDistribution.delta_components() == 0)
                        {
                            earlyExit = true;
                            return;
                        }

                        if (bsdfDistribution.has_non_delta_component())
                            radianceAtCurrentPathVertex += EvaluateRadianceByLightsAtVertex(
                                currentRay,
                                hit,
                                color_index,
                                sampler);


                        const auto bsdfSample = bsdfDistribution.generate_sample();
                        const auto bsdfDirection = bsdfSample.getValue().outgoingDirection;
                        const auto bsdfColor = bsdfSample.getValue().evaluate();
                        const auto bsdfPdf = color_real(bsdfSample.getPdf());

                        const bool entering = math::is_obtuse_angle(currentRay.direction(), hit.normal());

                        const auto samplePayload = radianceAtCurrentPathVertex * throughput;
                        catch_invalid(samplePayload);

                        integral += samplePayload;

                        catch_invalid(integral);

                        const auto geometricTerm = color_real(std::abs(math::dot(bsdfDirection, hit.normal())));

                        const auto vertexThroughput = bsdfSample.getIsDelta()
                                                          ? bsdfColor * geometricTerm / bsdfPdf
                                                          : bsdfColor * geometricTerm * color_real(math::oneOverPi) /
                                                          bsdfPdf;

                        const auto survive_probability = std::clamp(color::get_survive_importance(vertexThroughput),
                                                                    MIN_SURVIVE_PROBABILITY, MAX_SURVIVE_PROBABILITY);

                        // Termination by russian roulette
                        // a workaround since uniform_random_generator occasionally generates 1.0f when it should not.
                        if (sampler.Get1D() >= survive_probability)
                        {
                            earlyExit = true;
                            return;
                        }
                        else
                        {
                            throughput *= vertexThroughput / survive_probability;
                            catch_invalid(throughput);
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

                        if (!bsdfSample.getValue().isFullyTransparent)
                            accountForEmission = bsdfSample.getIsDelta();

                        const auto rt_bias = BiasForPosition(currentRay.origin());
                        hit = raytracer_.TraceRay(currentRay, rt_bias,
                                                  std::numeric_limits<space_real>::max(),
                                                  hit.object(),
                                                  hit.normal());
                    });
            }

            catch_invalid(integral);
            return integral;
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

        [[nodiscard]] static participating_media::optical_thickness_scalar_t ExtinctionForSamping(
            const participating_media::optical_thickness_t& extinction)
        {
            return participating_media::thickness_to_scalar(extinction); // Just take green for now
        }

        [[nodiscard]] color_rgb EvaluateInfinity(const ray3& ray, color_real emission_weight)
        {
            //return color_rgb(0, 0, 0);
            return sun_.EvaluateEmissionForDirection(ray.direction()) * emission_weight;
            //return color_rgb(0.5, 0.55, 0.8) + sun_.EvaluateEmissionForDirection(ray.direction()) * accountForEmission;
        }


        light_sample SampleLight(
            const media_interaction_scatter& scatter,
            const ray3& currentRay,
            const std::size_t color_index,
            math::Sampler<space_real>& sampler)
        {
            const auto& light_source = light_source_;
            light_sample light_sample{};
            light_source.DoWithDistribution(
                scatter.new_ray.origin(),
                sampler,
                [
                    this,
                    &light_source,
                    &scatter,
                    &sampler,
                    &currentRay,
                    &light_sample,
                    color_index
                ](const lights::light_distribution& lightDistribution)
                {
                    if (lightDistribution.delta_components() != 0 || lightDistribution.
                        has_non_delta_component())
                    {
                        const auto optionalLightSample = lightDistribution.generate_sample();

                        if (optionalLightSample.getValue())
                        {
                            const auto& lightSample = *optionalLightSample.getValue();

                            const ray3 light_ray{scatter.new_ray.origin(), lightSample.direction};

                            const auto light_ray_transmittance = EvaluateTransmittanceAlongSegment(
                                light_ray,
                                lightSample.distance - BIAS * lightSample.distance,
                                color_index,
                                sampler
                            );

                            if (!math::isZero(light_ray_transmittance))
                            {
                                auto lightValue = lightSample.evaluate();

                                const auto scattering_coeffs = scatter.phase_function(
                                    currentRay.direction(), lightSample.direction, color_index);


                                //const auto lightSamplePdf = color_real(optionalLightSample.getPdf());

                                auto radianceByLight =
                                        lightValue
                                        * scattering_coeffs
                                    // / lightSamplePdf
                                    ;

                                catch_invalid(radianceByLight);

                                light_sample.value = radianceByLight * light_ray_transmittance;
                                light_sample.pdf = color_real(optionalLightSample.getPdf());
                                light_sample.direction = lightSample.direction;
                                light_sample.scatter_dir_pdf = light_source.EvaluatePdfExperimental(scatter.new_ray);
                            }
                        }
                    }
                });
            return light_sample;
        }

        media_trace_result TraceMedia(
            participating_media::DynamicCompositeMedium& medium,
            const ray3& ray,
            space_real max_distance,
            const color_rgb& color_sampling_hint,
            const std::size_t color_index,
            math::Sampler<space_real>& sampler)
        {
            return TraceMediaOneByOne(medium, ray, max_distance, color_sampling_hint, color_index, sampler);
        }

        media_trace_result TraceMediaOneByOne(
            const participating_media::DynamicCompositeMedium& medium,
            const ray3& ray,
            space_real max_distance,
            const color_rgb& color_sampling_hint,
            const std::size_t color_index,
            math::Sampler<space_real>& sampler)
        {
            const auto color_pdf = 1;

            // const auto color_index = static_cast<size_t>(3 * sampler.Get1D());
            // const auto color_pdf = color::LUMINOCITY_WEIGHTS[color_index];

            // const auto random_var = std::min(color_real(0.99999), color_real(sampler.Get1D())); // to avoid getting 1 when casting to color_real. Sum of weights may also be less than 1, probably.
            // const auto [color_index, color_pdf] = color::sample_color_component(color_sampling_hint, random_var);

            assert(color_index < 3);

            auto distance_left = max_distance;
            auto majorant_transmittance = color_rgb::one();
            //majorant_transmittance[color_index] = 1 / color_pdf;

            catch_invalid(majorant_transmittance);

            auto current_ray = ray;
            for (std::size_t i = 0; ; ++i)
            {
                const auto [majorant_sampling_extinction, majorant_valid_distance] = medium.SampleMajorantExtinction(current_ray, distance_left, color_index);

                if (majorant_sampling_extinction == color_real(0) && majorant_valid_distance >= distance_left) {
                    return media_interaction_none{
                        majorant_transmittance
                    };
                }

                const auto step_distance = majorant_sampling_extinction > 0
                    ? math::sampleExponential(color_real(sampler.Get1D()), majorant_sampling_extinction)
                    : majorant_valid_distance
                ;

                if (majorant_valid_distance < distance_left) {
                    if (step_distance >= majorant_valid_distance) {
                        distance_left -= majorant_valid_distance;
                        // No interaction happened within valid distance. Just skip this step.
                        current_ray = current_ray.ray_along(majorant_valid_distance);
                        continue;
                    }
                    distance_left -= step_distance;
                } else {
                    distance_left -= step_distance;
                    if (distance_left <= space_real(0.0))
                    {
                        return media_interaction_none{
                            majorant_transmittance
                        };
                    }
                }

                const auto new_ray = current_ray.ray_along(step_distance);
                auto media_properties = medium.SampleProperties(new_ray.origin());

                const auto interaction_sigma = media_properties.absorption[color_index] + media_properties.scattering[
                    color_index];

                const auto random_value = sampler.Get1D() * majorant_sampling_extinction;
                if (random_value < media_properties.absorption[color_index]) {
                    return media_interaction_absorption{
                        .emission = media_properties.emission * majorant_transmittance,
                        };
                } else if (random_value < interaction_sigma) {
                    const auto scattering_sample = media_properties.scatter_generator(
                        current_ray.direction(),
                        color_index,
                        sampler);

                    assert(scattering_sample.pdf != color_real(0));
                    assert(std::isfinite(scattering_sample.pdf));

                    return media_interaction_scatter{
                        {new_ray.origin(), scattering_sample.direction},
                        majorant_transmittance,
                        scattering_sample.transmittance,
                        scattering_sample.pdf,
                        std::move(media_properties.phase_function),
                        std::move(media_properties.evaluate_pdf)
                    };
                }
                else
                {
                    // null scattering

                    if (sampler.Get1D() >= MAX_SURVIVE_PROBABILITY)
                    {
                        return media_interaction_absorption{
                            .emission = color_rgb::zero(),
                            };
                    }
                    else
                    {
                        majorant_transmittance = majorant_transmittance /
                            MAX_SURVIVE_PROBABILITY;

                        catch_invalid(majorant_transmittance);
                    }

                    current_ray = new_ray;
                }
            }
        }

        color_rgb EvaluateTransmittanceAlongSegment(
            const ray3& ray,
            const space_real& distance,
            const std::size_t color_index,
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

                auto media_result = TraceMedia(shadowMedia_, currentRay, limited_obstruction_free_distance, throughput, color_index, sampler);

                if (const auto absorption = std::get_if<media_interaction_absorption>(&media_result))
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

                const auto survive_probability = std::clamp(color::get_survive_importance(surface_transmittance),
                                                            MIN_SURVIVE_PROBABILITY, MAX_SURVIVE_PROBABILITY);

                if (math::isZero(surface_transmittance) || sampler.Get1D() >= survive_probability)
                {
                    throughput = color_rgb::zero();
                    break;
                }
                else
                {
                    throughput *= surface_transmittance / survive_probability;
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
            const std::size_t color_index,
            math::Sampler<space_real>& sampler)
        {
            color_rgb radianceAtCurrentPathVertex{color_rgb::zero()};
            const lights::LightingContext context{hit.object(), hit.point(), hit.normal(), BIAS, 1, false};

            const auto& light = light_source_;
            light.DoWithDistribution(
                context,
                sampler,
                [&](const lights::light_distribution& lightDistribution)
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
                                    color_index,
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
    };
}
