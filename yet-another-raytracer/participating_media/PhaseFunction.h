#pragma once

#include <Sampler.h>

#include "media_common.h"

#include <concepts>

namespace participating_media
{
    class PhaseFunction
    {
    public:
        virtual ~PhaseFunction() = default;

        [[nodiscard]] virtual scattering_event Sample(const vector3& incident_direction,
                                                      math::Sampler<space_real>& sampler) const = 0;

        [[nodiscard]] virtual spectral_coeffs Evaluate(const vector3& incident_direction,
                                                       const vector3& outgoing_direction) const = 0;
    };

    template <class T>
    concept CPhaseFunction = std::derived_from<T, PhaseFunction>;
}
