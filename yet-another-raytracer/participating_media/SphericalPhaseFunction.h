#pragma once

#include "PhaseFunction.h"

namespace participating_media
{
    class SphericalPhaseFunction final : public PhaseFunction
    {
    public:
        [[nodiscard]] scattering_event Sample(const vector3& incident_direction, math::Sampler<space_real>& sampler) const override
        {
            auto direction = math::sphericalRand<space_real>(sampler);

            return scattering_event{
                std::move(direction),
                spectral_coeffs::one()
            };
        }

        [[nodiscard]] spectral_coeffs Evaluate(const vector3& incident_direction,
                                               const vector3& outgoing_direction) const override
        {
            return spectral_coeffs::fill(space_real(0.25) * std::numbers::inv_pi_v<space_real>);
        }
    };
}
