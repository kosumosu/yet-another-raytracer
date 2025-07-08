#pragma once

#include "LightingContext.h"
#include "LightSource.h"
#include "Types.h"

namespace lights
{
    class SunLightSource final : public LightSource
    {
        using lighting_functional_distribution = FunctionalDistribution<
            std::optional<light_sample>, vector3, space_real>;

        const space_real half_angular_size_cos_;
        const space_real pdf_;
        const color_rgb color_per_sample_;
        const matrix3 direction_transform_;
        const vector3 direction_;

    public:
        constexpr static space_real earth_to_sun_distance = 149600000.0;
        constexpr static space_real sun_radius = 696000.0;
        const space_real real_sun_average_angular_size_radians = std::atan(
            sun_radius / earth_to_sun_distance) * 2.0;

        SunLightSource(
            const color_rgb& color,
            const space_real& angular_size_radians,
            const space_real& azimuth_radians,
            const space_real& elevation_radians)
            : half_angular_size_cos_(std::cos(angular_size_radians * space_real(0.5)))
              , pdf_(math::conicalRandPdf<space_real>(half_angular_size_cos_))
              , color_per_sample_(color * pdf_)
              , direction_transform_(GenerateTransformMatrix(azimuth_radians, elevation_radians))
              , direction_(math::from_angles(azimuth_radians, elevation_radians))
        {
        }

        void DoWithDistribution(const LightingContext& context, math::Sampler<space_real>& sampler,
                                const distibution_func& job) const noexcept override
        {
            DoWithDistribution(context.getPoint(), sampler, job);
        }

        void DoWithDistribution(const vector3& point, math::Sampler<space_real>& sampler,
                                const distibution_func& job) const noexcept override
        {
            job(lighting_functional_distribution(
                0U,
                true,
                [&, this]
                {
                    const auto dir = math::conicalRand(sampler, half_angular_size_cos_);
                    auto direction = direction_transform_ * dir;

                    return math::random_sample<std::optional<light_sample>, space_real>(
                        light_sample(
                            std::move(direction),
                            std::numeric_limits<space_real>::max(),
                            [this]
                            {
                                return color_per_sample_;
                            }
                        ),
                        pdf_,
                        false);
                }
            ));
        }

        [[nodiscard]] space_real EvaluatePdfExperimental(const ray3& ray) const noexcept override
        {
            const auto cos_theta = math::dot(ray.direction(), direction_);
            const bool hits = cos_theta > half_angular_size_cos_;

            return hits * pdf_;
        }

        [[nodiscard]] color_real GetApproximateTotalPower() const noexcept override
        {
            return color::get_importance(color_per_sample_ / pdf_);
        }


        [[nodiscard]] color_rgb EvaluateEmissionForDirection(const vector3& direction) const noexcept
        {
            const bool hits = math::dot(direction, direction_) > half_angular_size_cos_;
            return color_per_sample_ * hits;
        }

    private:
        constexpr static matrix3 GenerateTransformMatrix(const space_real& azimuth_radians,
                                                         const space_real& elevation_radians) noexcept
        {
            return math::rotate3x3(vector3(0.0, 0.0, 1.0), azimuth_radians)
                * math::rotate3x3(vector3(0.0, 1.0, 0.0), -elevation_radians);
        }
    };
}
