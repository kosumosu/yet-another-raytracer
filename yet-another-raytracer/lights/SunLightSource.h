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

        const space_real disc_radius_; // a disc 1 unit away, with same angular size
        const space_real inv_disc_area_;
        const color_rgb color_per_unit_area_;
        const matrix3 direction_transform_;

        const vector3 direction_;
        const space_real half_angular_size_cos_;

    public:
        constexpr static space_real earth_to_sun_distance = 149600000.0;
        constexpr static space_real sun_radius = 696000.0;
        constexpr static space_real real_sun_average_angular_size_radians = std::atan(
            sun_radius / earth_to_sun_distance) * 2.0;

        SunLightSource(
            const color_rgb& color,
            const space_real& angular_size_radians,
            const space_real& azimuth_radians,
            const space_real& elevation_radians)
            : disc_radius_(std::atan(angular_size_radians * space_real(0.5)))
              , inv_disc_area_(space_real(1) / (std::numbers::pi_v<space_real> * disc_radius_ * disc_radius_))
              , color_per_unit_area_(color * inv_disc_area_)
              , direction_transform_(GenerateTransformMatrix(azimuth_radians, elevation_radians))
              , direction_(math::from_angles(azimuth_radians, elevation_radians))
              , half_angular_size_cos_(std::cos(angular_size_radians * space_real(0.5)))
        {
        }

        void DoWithDistribution(const LightingContext& context, math::Sampler<space_real>& sampler,
                                const distibution_func& job) const override
        {
            DoWithDistribution(context.getPoint(), sampler, job);
        }

        void DoWithDistribution(const vector3& point, math::Sampler<space_real>& sampler,
                                const distibution_func& job) const override
        {
            job(lighting_functional_distribution(
                0U,
                true,
                [&, this]
                {
                    const auto point_on_unit_circle = math::circularRand<space_real>(sampler);
                    const auto point_on_scaled_circle = point_on_unit_circle * disc_radius_;
                    const auto point_on_scaled_circle3d = vector3(1.0, point_on_scaled_circle[0],
                                                                  point_on_scaled_circle[1]);

                    const auto direction = direction_transform_ * point_on_scaled_circle3d;

                    return math::random_sample<std::optional<light_sample>, space_real>(
                        light_sample(
                            direction,
                            std::numeric_limits<space_real>::max(),
                            [this]
                            {
                                return color_per_unit_area_;
                            }
                        ),
                        inv_disc_area_,
                        false);
                }
            ));
        }

        [[nodiscard]] color_real GetApproximateTotalPower() const override
        {
            return color::get_importance(color_per_unit_area_ / inv_disc_area_);
        }


        [[nodiscard]] color_rgb EvaluateEmissionForDirection(const vector3& direction) const
        {
            const bool hits = math::dot(direction, direction_) > half_angular_size_cos_;
            return color_per_unit_area_ * hits;
        }

    private:
        constexpr static matrix3 GenerateTransformMatrix(const space_real& azimuth_radians,
                                                         const space_real& elevation_radians)
        {
            return math::rotate3x3(vector3(0.0, 0.0, 1.0), azimuth_radians)
                * math::rotate3x3(vector3(0.0, 1.0, 0.0), -elevation_radians);
        }
    };
}
