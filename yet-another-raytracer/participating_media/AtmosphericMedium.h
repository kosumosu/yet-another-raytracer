#pragma once

#include "ParticipatingMedium.h"
#include "PhaseFunction.h"

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

        const TPhaseFunction& phase_function_;

    public:
        AtmosphericMedium(
            space_real planet_radius,
            space_real scale_height,
            vector3 planet_center,
            optical_thickness_t absorption,
            optical_thickness_t scattering,
            const TPhaseFunction& phase_function
        )
            : planet_radius_(planet_radius)
              , inv_scale_height_(1 / scale_height)
              , planet_center_(std::move(planet_center))
              , absorption_(std::move(absorption))
              , scattering_(std::move(scattering))
              , phase_function_(std::move(phase_function))
        {
        }

        [[nodiscard]] optical_thickness_t
        SampleMajorantExtinction(const ray3& ray, space_real max_distance) const override
        {
            // Find max majorant along the ray. It's maximum at the point, closest to planet center.
            const auto origin_to_planet_center = planet_center_ - ray.origin();
            const auto distance_to_point_closest_to_planet = math::dot(origin_to_planet_center, ray.direction());
            const auto clamped_distance = std::clamp(distance_to_point_closest_to_planet, space_real(0), max_distance);
            const auto point_closest_to_planet = ray.point_along(clamped_distance);
            const auto density = DensityAtPoint(point_closest_to_planet);
            return (scattering_ + absorption_) * color_real(density);
        }


        [[nodiscard]] medium_properties SampleProperties(const vector3& point) const override
        {
            const auto density = color_real(DensityAtPoint(point));
            return {
                absorption_ * density,
                scattering_ * density,
                spectral_coeffs::zero(),
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
        [[nodiscard]] color_real DensityAtPoint(const vector3& point) const
        {
            const auto point_to_center = point - planet_center_;
            const auto distance_to_center = math::length(point_to_center);
            const auto scaled_height = std::max(space_real(0),
                                                (distance_to_center - planet_radius_) * inv_scale_height_);
            const auto density = scaled_height < 20 ? std::exp(-color_real(scaled_height)) : 0;
            return density;
        }
    };
}
