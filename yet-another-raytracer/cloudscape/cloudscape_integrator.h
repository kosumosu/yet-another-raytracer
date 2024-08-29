#pragma once

#include "atmosphere_math.h"

#include "color_functions.hpp"

#include "prepared_scene.h"

#include "integrators/RayIntegrator.h"
#include "ray.hpp"

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

    class CloudscapeIntegrator final : public RayIntegrator
    {
        constexpr static space_real EPS = std::numeric_limits<float>::epsilon() * 1.0f;
        constexpr static float MAX_DIST = std::numeric_limits<float>::max();

        const prepared_scene scene_;
        //std::vector<sample> samples_;

    public:
        explicit CloudscapeIntegrator(prepared_scene scene): scene_(std::move(scene))
        {
        }

        color_rgb EvaluateRay(const ray3& ray, space_real bias,
                              math::Sampler<space_real>& sampler) override
        {
            //samples_.clear();

            const auto lower_cloud_bound2 = square(
                scene_.scene.planet.planetradius + scene_.scene.clouds.height - scene_.scene.clouds.thickness);
            const auto upper_cloud_bound2 = square(
                scene_.scene.planet.planetradius + scene_.scene.clouds.height + scene_.scene.clouds.thickness);

            color_rgb total_color = color_rgb::zero();
            color_rgb transp = color_rgb::fill(1.0);

            auto current_pos = ray.origin(); // NP
            auto total_distance = 0.0f;

            for (size_t i = 0; i < scene_.scene.rendering.samples; ++i)
            {
                const math::ray current_ray{current_pos, ray.origin()};
                const auto dist_to_planet_center2 = math::length2(current_pos - scene_.planet_pos);

                if (dist_to_planet_center2 < lower_cloud_bound2 || dist_to_planet_center2 > upper_cloud_bound2)
                {
                    const auto clouds_hit = dist_to_planet_center2 < lower_cloud_bound2
                                                ? scene_.lower_cloud_bound.FindHit(
                                                    current_ray, 0.0f, MAX_DIST)
                                                : scene_.upper_cloud_bound.FindHit(
                                                    current_ray, 0.0f, MAX_DIST);

                    const auto planet_hit = scene_.planet.FindHit(current_ray, 0.0f, MAX_DIST);

                    if (planet_hit.has_occurred() &&
                        (
                            clouds_hit.has_occurred() && planet_hit.distance() < clouds_hit.distance()
                            || !clouds_hit.has_occurred()
                        ))
                    {
                        current_pos = planet_hit.point(); // bias ?

                        const auto sun_light_color = EvaluateLightRay({current_pos, scene_.sun_direction});
                        const auto geom_factor = math::dot(planet_hit.normal(), scene_.sun_direction);
                        const auto vertex_color = scene_.ground_color * (scene_.ambient_light_color + sun_light_color *
                            geom_factor);

                        total_color += vertex_color * transp;

                        // samples_.emplace_back(
                        //     currnet_pos,
                        //     color_rgbx{total_color, 0.0f},
                        //     samples_.size() + 1,
                        //     samples_.size() - 1,
                        //     scene_.scene.rendering.step
                        // );
                        break;
                    }
                    else if (clouds_hit.has_occurred() &&
                        (
                            planet_hit.has_occurred() && clouds_hit.distance() < planet_hit.distance()
                            || !planet_hit.has_occurred()
                        ))
                    {
                        current_pos = clouds_hit.point() + clouds_hit.point() * EPS;
                        total_distance += clouds_hit.distance();
                    }
                    else
                    {
                        total_color += EvaluateAtmosphere(current_ray) * transp;
                        break; // went to space
                    }
                }
                else
                {
                    // if (!samples_.empty())
                    // {
                    // const auto distance = samples_[samples_.size() - 1].distance_after;
                    const auto distance = scene_.scene.rendering.step;
                    current_pos = current_pos + ray.direction() * distance;
                    total_distance += distance;
                    // }
                    // else
                    // {
                    //     current_pos += ray.direction() * current_pos * EPS;
                    //}

                    // const auto actual_step = scene_.scene.rendering.step * std::pow(
                    //     total_distance, scene_.scene.optimizations.optimizations_stepfalloff);

                    const auto color = SampleVolume({ current_pos, ray.direction()});

                    const auto optical_density = color[3] * scene_.scene.rendering.step;
                    const auto sample_transparency = std::exp(-optical_density);
                    const auto sample_opacity = 1.0f - sample_transparency;

                    total_color += color.reduce() * sample_opacity * transp;
                    transp *= sample_transparency;

                    // samples_.emplace_back(
                    //         currnet_pos,
                    //         color,
                    //         samples_.size() + 1,
                    //         samples_.size() - 1,
                    //         scene_.scene.rendering.step
                    //     );

                    if (color::get_importance(transp) < scene_.scene.optimizations.optimizations_cutoffthresshold)
                        break;
                }
            }


        }

    private:
        [[nodiscard]] color_rgb EvaluateLightRay(const ray3& ray) const
        {
            return color_rgb::fill(0.5);
        }

        // result[3] is density, not transparency!
        [[nodiscard]] color_rgbx SampleVolume(const ray3& ray) const
        {
            return color_rgbx::fill(0.5);
        }

        // should include sun
        [[nodiscard]] color_rgb EvaluateAtmosphere(const ray3& ray) const
        {
            return color_rgb(0.5, 0.55, 0.8);

            // auto beta_rayleigh = scene_.beta_total_rayleigh;
            // auto beta_mie = scene_.beta_total_mie;
            //
            //
        }

    };
}
