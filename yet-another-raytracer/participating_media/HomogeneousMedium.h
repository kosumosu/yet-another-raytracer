#pragma once

#include "Sampler.h"

#include "PhaseFunction.h"
#include "ParticipatingMedium.h"

namespace participating_media
{
    template <CPhaseFunction TPhaseFunction>
    class HomogeneousMedium final : public ParticipatingMedium
    {
        const optical_thickness_t absorption_;
        const optical_thickness_t scattering_;
        const optical_thickness_t majorant_;
        const spectral_coeffs emission_;
        const TPhaseFunction& phase_function_;

    public:
        HomogeneousMedium(optical_thickness_t absorption, optical_thickness_t scattering, spectral_coeffs emission,
                          const TPhaseFunction& phase_function)
            : absorption_(std::move(absorption))
              , scattering_(std::move(scattering))
              , majorant_(absorption_ + scattering_)
              , emission_(std::move(emission))
              , phase_function_(phase_function)
        {
        }

        [[nodiscard]] optical_thickness_t
        SampleMajorantExtinction(const ray3& ray, space_real max_distance) const override
        {
            return majorant_;
        }

        [[nodiscard]] medium_properties SampleProperties(const vector3& point) const override
        {
            return {
                absorption_,
                scattering_,
                emission_,
                [this](
                const vector3& incident_direction,
                std::size_t color_index,
                math::Sampler<space_real>& sampler)
                {
                    return phase_function_.Sample(incident_direction, sampler);
                },
                [this](const vector3& incident_direction,
                       const vector3& outgoing_direction,
                       std::size_t color_index)
                {
                    return phase_function_.Evaluate(incident_direction, outgoing_direction);
                },
                [this](const vector3& incident_direction,
                       const vector3& outgoing_direction,
                       std::size_t color_index)
                {
                    return phase_function_.EvaluatePdf(incident_direction, outgoing_direction);
                }
            };
        }
    };
}
