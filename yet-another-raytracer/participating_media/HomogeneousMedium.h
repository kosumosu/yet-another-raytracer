#pragma once

#include "Sampler.h"

#include "ParticipatingMedium.h"

namespace participating_media
{
    class HomogeneousMedium final : public ParticipatingMedium
    {
        optical_thickness_t absorption_;
        optical_thickness_t scattering_;
        optical_thickness_t majorant_;
        spectral_coeffs emission_;

    public:
        HomogeneousMedium(optical_thickness_t absorption, optical_thickness_t scattering, spectral_coeffs emission)
            : absorption_(std::move(absorption))
              , scattering_(std::move(scattering))
              , majorant_(absorption_ + scattering_)
              , emission_(std::move(emission))
        {
        }

        [[nodiscard]] optical_thickness_t SampleMajorantExtinction(const ray3& ray, space_real max_distance) const override
        {
            return majorant_;
        }

        [[nodiscard]] medium_properties SampleProperties(const vector3& point) const override
        {
            return {
                absorption_,
                scattering_,
                emission_,
                [](math::Sampler<space_real>& sampler, const extinction_to_scalar_t& extinction_to_scalar)
                {
                    auto direction = math::sphericalRand<space_real>(sampler);

                    return scattering_event{
                        std::move(direction),
                        spectral_coeffs::one()
                    };
                },
                [](const vector3& incident_direction,
                   const vector3& outgoing_direction)
                {
                    return spectral_coeffs::fill(space_real(0.25) * std::numbers::inv_pi_v<space_real>);
                }
            };
        }

    };
}
