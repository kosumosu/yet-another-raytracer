#pragma once

#include "ParticipatingMedium.h"
#include "PhaseFunction.h"

#include <limits>

namespace participating_media
{
    template <CPhaseFunction TPhaseFunction>
    class AtmosphericMedium final : public ParticipatingMedium
    {
        const space_real planet_radius_;
        const space_real inv_scale_height_;
        const vector3 planet_center_;

        const optical_thickness_t absorption_; // at ground level
        const optical_thickness_t scattering_;
        const spectral_coeffs emission_;

        const TPhaseFunction& phase_function_;

    public:
        AtmosphericMedium(
            space_real planet_radius,
            space_real scale_height,
            vector3 planet_center,
            optical_thickness_t absorption,
            optical_thickness_t scattering,
            spectral_coeffs emission,
            const TPhaseFunction& phase_function
        )
            : planet_radius_(planet_radius)
              , inv_scale_height_(1 / scale_height)
              , planet_center_(std::move(planet_center))
              , absorption_(std::move(absorption))
              , scattering_(std::move(scattering))
              , emission_(std::move(emission))
              , phase_function_(std::move(phase_function))
        {
        }

        [[nodiscard]] majorant_sample_result
        SampleMajorantExtinction(const ray3& ray, space_real max_distance) const override {
            // Find max majorant along the ray. It's maximum at the point, closest to planet center.
            const auto origin_to_planet_center = planet_center_ - ray.origin();
            const auto distance_to_point_closest_to_planet = math::dot(origin_to_planet_center, ray.direction());
            const auto clamped_distance = std::clamp(distance_to_point_closest_to_planet, space_real(0), max_distance);
            const auto point_closest_to_planet = ray.point_along(clamped_distance);
            const auto density = RelativeDensityAtPoint(point_closest_to_planet);
            return {
                (scattering_ + absorption_) * density,
                    std::numeric_limits<space_real>::max(),
                    };
        }

        [[nodiscard]] single_majorant_sample_result SampleMajorantExtinction(const ray3 &ray, space_real max_distance,
            const std::size_t color_index) const override {

            if constexpr (false) {
                const auto origin_to_planet_center = planet_center_ - ray.origin();
                const auto distance_to_point_closest_to_planet = math::dot(origin_to_planet_center, ray.direction());
                const auto clamped_distance = std::clamp(distance_to_point_closest_to_planet, space_real(0), max_distance);
                const auto point_closest_to_planet = ray.point_along(clamped_distance);
                const auto density = RelativeDensityAtPoint(point_closest_to_planet);
                return {
                    (scattering_[color_index] + absorption_[color_index]) * density,
                        std::numeric_limits<space_real>::max(),
                        };
            } else {
                const auto origin_to_planet_center = planet_center_ - ray.origin();
                const auto distance_to_point_closest_to_planet = math::dot(origin_to_planet_center, ray.direction());

                if (distance_to_point_closest_to_planet > 0) {
                    const auto clamped_distance = std::min(distance_to_point_closest_to_planet, max_distance);
                    const auto point_closest_to_planet = ray.point_along(clamped_distance);
                    const auto relative_density = RelativeDensityAtPoint(point_closest_to_planet);
                    const auto absolute_density = (scattering_[color_index] + absorption_[color_index]) * relative_density;

                    const auto halfway_distance = clamped_distance * space_real(0.5);
                    const auto halfway_point = ray.point_along(halfway_distance);
                    const auto halfway_relative_density = RelativeDensityAtPoint(halfway_point);
                    const auto halfway_absolute_density = (scattering_[color_index] + absorption_[color_index]) * halfway_relative_density;

                    const auto endpoint_average_step = absolute_density > 0 ? color_real(1.0) / absolute_density : std::numeric_limits<space_real>::max();
                    const auto halfway_average_step = EstimateAverageExponentialStepSize(halfway_absolute_density, halfway_distance);

                    if (halfway_average_step > endpoint_average_step) {
                        return {
                            halfway_absolute_density,
                                halfway_distance,
                                };
                    } else {
                        return {
                            absolute_density,
                                std::numeric_limits<space_real>::max(),
                                };
                    }


                } else {
                    const auto& point_closest_to_planet = ray.origin();
                    const auto relative_density = RelativeDensityAtPoint(point_closest_to_planet);
                    const auto absolute_density = (scattering_[color_index] + absorption_[color_index]) * relative_density;

                    return {
                        absolute_density,
                            std::numeric_limits<space_real>::max(),
                            };
                }
            }
        }


        [[nodiscard]] medium_properties SampleProperties(const vector3& point) const override
        {
            const auto density = RelativeDensityAtPoint(point);
            return {
                absorption_ * density,
                scattering_ * density,
                emission_,
                [this](
                const vector3& incident_direction,
                std::size_t color_index,
                math::Sampler<space_real>& sampler)
                {
                    return phase_function_.Sample(incident_direction, sampler);
                },
                [this](
                const vector3& incident_direction,
                const vector3& outgoing_direction,
                std::size_t color_index)
                {
                    return phase_function_.Evaluate(incident_direction, outgoing_direction);
                },
                [this](
                const vector3& incident_direction,
                const vector3& outgoing_direction,
                std::size_t color_index)
                {
                    return phase_function_.EvaluatePdf(incident_direction, outgoing_direction);
                }
            };
        }

    private:
        [[nodiscard]] color_real RelativeDensityAtPoint(const vector3& point) const
        {
            const auto point_to_center = point - planet_center_;
            const auto distance_to_center = math::length(point_to_center);
            const auto scaled_height = std::max(space_real(0),
                                                (distance_to_center - planet_radius_) * inv_scale_height_);
            //const auto density = scaled_height < 20 ? std::exp(-color_real(scaled_height)) : 0;
            const auto density = std::exp(-color_real(scaled_height));
            return density;
        }

        [[nodiscard]] static space_real EstimateAverageExponentialStepSize(const color_real& density, const space_real& step_limit) {
            return density > 0 ? (space_real(1) - std::exp(-density * step_limit)) / density : step_limit;
        }
    };
}
