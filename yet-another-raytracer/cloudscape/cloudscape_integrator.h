#pragma once

#include "atmosphere_math.h"

#include "color/color_functions.hpp"

#include "prepared_scene.h"

#include "integrators/RayIntegrator.h"
#include "math/ray.hpp"

#include <PerlinNoise.hpp>

#include <limits>

namespace cloudscape
{
    struct sample
    {
        vector3 position;
        color_rgbx color;
        uint32_t next_sample_num;
        uint32_t prev_sample_num;
        float distance_after;
    };

    struct shadow_sample
    {
        vector3 position;
        float color;
        uint32_t next_sample_num;
        uint32_t prev_sample_num;
        float distance_after;
    };

    struct atmospheric_sample
    {
        color_rgb inscatter;
        color_rgb transparency;
    };

    class CloudscapeIntegrator final : public RayIntegrator
    {
        constexpr static space_real EPS = std::numeric_limits<float>::epsilon() * 1.0f;
        constexpr static float MAX_DIST = std::numeric_limits<float>::max();

        const prepared_scene scene_;
        const siv::PerlinNoise noise_;
        //std::vector<sample> samples_;

    public:
        explicit CloudscapeIntegrator(prepared_scene scene)
            : scene_(std::move(scene))
              , noise_{1234}
        {
        }

        color_rgb EvaluateRay(const ray3& ray, space_real bias,
                              math::Sampler<space_real>& sampler) override
        {
            return EvaluateRayImpl(ray, bias, sampler, 2);
        }

        color_rgb EvaluateRayImpl(const ray3& ray, space_real bias,
                              math::Sampler<space_real>& sampler, size_t depth_left) const
        {
            if (depth_left == 0)
                return color_rgb::zero();

            const auto lower_cloud_bound2 = square(scene_.lower_cloud_bound.radius());
            const auto upper_cloud_bound2 = square(scene_.upper_cloud_bound.radius());

            color_rgb total_color = color_rgb::zero();
            color_rgb transp = color_rgb::fill(1.0);

            auto current_pos = ray.origin(); // NP
            auto total_distance = 0.0f;

            for (size_t i = 0; i < scene_.scene.rendering.samples; ++i)
            {
                const math::ray current_ray{current_pos, ray.direction()};
                const auto dist_to_planet_center2 = math::length2(current_pos - scene_.planet.center());

                if (dist_to_planet_center2 < lower_cloud_bound2 || dist_to_planet_center2 > upper_cloud_bound2)
                {
                    const auto clouds_hit = dist_to_planet_center2 < lower_cloud_bound2
                                                ? scene_.lower_cloud_bound.FindHit(
                                                    current_ray, 0.0f, MAX_DIST)
                                                : scene_.upper_cloud_bound.FindHit(
                                                    current_ray, 0.0f, MAX_DIST);

                    const auto planet_hit = scene_.planet.FindHit(current_ray, 0.1f, MAX_DIST);

                    if (planet_hit.has_occurred() &&
                        (
                            clouds_hit.has_occurred() && planet_hit.distance() < clouds_hit.distance()
                            || !clouds_hit.has_occurred()
                        ))
                    {
                        const auto atmos_sample = EvaluateAtmosphere(current_pos, planet_hit.point());

                        total_color += atmos_sample.inscatter * transp;
                        transp *= atmos_sample.transparency;

                        const auto sun_light_color = EvaluateLightRay({planet_hit.point(), scene_.sun_direction}, bias, sampler, depth_left - 1);
                        const auto geom_factor = std::max(space_real(0.0),
                                                          math::dot(planet_hit.normal(), scene_.sun_direction));
                        constexpr auto norm_factor = std::numbers::inv_pi_v<color_real>;
                        const auto vertex_color = scene_.ground_color * (scene_.ambient_light_color + norm_factor * sun_light_color *
                            geom_factor);

                        total_color += vertex_color * transp;

                        break;
                    }
                    else if (clouds_hit.has_occurred() &&
                        (
                            planet_hit.has_occurred() && clouds_hit.distance() < planet_hit.distance()
                            || !planet_hit.has_occurred()
                        ))
                    {
                        const auto atmos_sample = EvaluateAtmosphere(current_pos, clouds_hit.point());
                        total_color += atmos_sample.inscatter * transp;
                        transp *= atmos_sample.transparency;

                        current_pos = clouds_hit.point() + clouds_hit.point() * EPS;
                        total_distance += clouds_hit.distance();
                    }
                    else
                    {
                        const auto atmos_sample = EvaluateAtmosphere(current_ray);
                        total_color += atmos_sample.inscatter * transp;
                        transp *= atmos_sample.transparency;
                        break; // went to space
                    }
                }
                else
                {
                    const auto distance = scene_.scene.rendering.step;
                    const auto new_pos = current_pos + ray.direction() * distance;

                    const auto atmos_sample = EvaluateAtmosphere(current_pos, new_pos);
                    total_color += atmos_sample.inscatter * transp;
                    transp *= atmos_sample.transparency;

                    current_pos = new_pos;
                    total_distance += distance;

                    const auto sun_light_color = EvaluateLightRay({current_pos, scene_.sun_direction}, bias, sampler, depth_left - 1);
                    const auto color = scene_.cloud_color * sun_light_color * CloudPhaseFunction(
                        math::dot(scene_.sun_direction, current_ray.direction()));

                    const auto density = SampleVolume(current_pos);

                    const auto optical_density = density * scene_.scene.rendering.step;
                    const auto sample_transparency = std::exp(-optical_density);
                    const auto sample_opacity = 1.0f - sample_transparency;

                    total_color += color * sample_opacity * transp;
                    transp *= sample_transparency;

                    if (color::get_importance(transp) < scene_.scene.optimizations.cutoffthresshold)
                        break;
                }
            }

            return total_color;
        }

        // TODO get rid of this
        statistics::Stats getStats() const
        {
            return {};
        }

    private:
        [[nodiscard]] color_rgb EvaluateLightRay(const ray3& ray, space_real bias,
                              math::Sampler<space_real>& sampler, size_t depth_left) const
        {
            return EvaluateRayImpl(ray, bias, sampler, depth_left);
            //return color_rgb::fill(0.5);
        }

        // result[3] is density, not transparency!
        [[nodiscard]] space_real SampleVolume(const vector3& position) const
        {
            const auto scaled_pos = position / scene_.scene.noise.size;
            //return scene_.scene.clouds.fog * std::max(space_real(0.0), space_real(noise_.normalizedOctave3D(scaled_pos[0], scaled_pos[1], scaled_pos[2], 2)) - scene_.scene.clouds.coverage);

            return
                scene_.scene.clouds.fog
                * std::max(space_real(0.0),
                           space_real(
                               space_real(scene_.scene.clouds.coverage - 0.5) * space_real(2.0) + noise_.
                               normalizedOctave3D(scaled_pos[0], scaled_pos[1], scaled_pos[2],
                                                  int(scene_.scene.noise.detail),
                                                  scene_.scene.noise.multiplier)));
        }

        [[nodiscard]] space_real CloudPhaseFunction(space_real cosTheta) const
        {
            // Uniform for now
            return space_real(0.25) * std::numbers::inv_pi_v<space_real>;
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
            auto transparency = math::exp(-color_rgb(0.00010, 0.00015, 0.0002) * math::length(end - start));
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
