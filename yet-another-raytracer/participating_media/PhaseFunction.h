#pragma once

#include <Sampler.h>

#include "media_common.h"

namespace participating_media
{
    struct scattering_event
    {
        vector3 direction;
        spectral_coeffs transmittance;
    };

    class PhaseFunction
    {
    public:
        virtual ~PhaseFunction() = default;

        [[nodiscard]] virtual scattering_event Sample(math::Sampler<space_real>& sampler) const = 0;

        [[nodiscard]] virtual spectral_coeffs Evaluate(const space_real& thetaCos) const = 0;
    };
}
