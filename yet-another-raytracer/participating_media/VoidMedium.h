#pragma once

#include "ParticipatingMedium.h"

namespace participating_media
{
    class VoidMedium final : public ParticipatingMedium
    {
    public:
        [[nodiscard]] optical_thickness_t SampleMajorantExtinction(
            const ray3& ray,
            space_real max_distance) const override
        {
            return optical_thickness_t::zero();
        }

        [[nodiscard]] medium_properties SampleProperties(const vector3& point) const override
        {
            return {
                optical_thickness_t::zero(),
                optical_thickness_t::zero(),
                spectral_coeffs::zero(),
                [](const vector3& incident_direction, std::size_t color_index, auto& sampler)
                {
                    return scattering_event{
                        vector3::zero(),
                        spectral_coeffs::zero(),
                        color_real(1)
                    };
                },
                [](const vector3& incident_direction,
                   const vector3& outgoing_direction,
                   std::size_t color_index)
                {
                    return (incident_direction == outgoing_direction)
                               ? spectral_coeffs::one()
                               : spectral_coeffs::zero();
                },
                [](const vector3& incident_direction,
                   const vector3& outgoing_direction,
                   std::size_t color_index)
                {
                    return space_real(incident_direction == outgoing_direction);
                }
            };
        }
    };
}
