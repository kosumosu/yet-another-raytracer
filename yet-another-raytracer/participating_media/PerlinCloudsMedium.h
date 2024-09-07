#pragma once

#include "ParticipatingMedium.h"
#include "Sampler.h"


#include <PerlinNoise.hpp>


namespace participating_media
{
    template <class TFunc>
    concept CDensityMultiplier = std::invocable<TFunc, const vector3&>;

    template <CPhaseFunction TPhaseFunction, CDensityMultiplier TDensityMultiplier>
    //template <class TDensityMultiplier>
    class PerlinCloudsMedium final : public ParticipatingMedium
    {
        const optical_thickness_t absorption_;
        const optical_thickness_t scattering_;
        const optical_thickness_t majorant_;
        const spectral_coeffs emission_;

        const space_real inv_size_;
        const space_real coverage_;
        const size_t octaves_;
        const space_real multiplier_;

        const TPhaseFunction& phase_function_;
        TDensityMultiplier density_multiplier_;

        const siv::PerlinNoise noise_;

    public:
        PerlinCloudsMedium(
            optical_thickness_t absorption,
            optical_thickness_t scattering,
            spectral_coeffs emission,
            const space_real& size,
            space_real coverage,
            size_t octaves,
            space_real multiplier,
            TDensityMultiplier density_multiplier,
            std::size_t seed,
            const TPhaseFunction& phase_function)
            : absorption_(std::move(absorption))
              , scattering_(std::move(scattering))
              , majorant_(absorption_ + scattering_)
              , emission_(std::move(emission))
              , inv_size_(1.0 / size)
              , coverage_(coverage)
              , octaves_(octaves)
              , multiplier_(multiplier)
              , density_multiplier_(std::move(density_multiplier))
              , noise_(seed)
              , phase_function_(phase_function)
        {
        }

        [[nodiscard]] optical_thickness_t SampleMajorantExtinction(
            const ray3& ray,
            space_real max_distance) const override
        {
            return majorant_;
        }

        [[nodiscard]] medium_properties SampleProperties(const vector3& point) const override
        {
            const auto scaled_pos = point * inv_size_;
            const auto final_pos = scaled_pos;

            const auto noise_density = std::max(space_real(0.0),
                                                space_real(
                                                    space_real(coverage_ - 0.5) * space_real(2.0) + noise_.
                                                    normalizedOctave3D(final_pos[0], final_pos[1], final_pos[2],
                                                                       std::int32_t(octaves_),
                                                                       multiplier_)));

            const auto density = noise_density * density_multiplier_(point);

            // const space_real density = 1.0;

            return {
                absorption_ * density,
                scattering_ * density,
                emission_,
                [this](const vector3& incident_direction, math::Sampler<space_real>& sampler)
                {
                    return phase_function_.Sample(incident_direction, sampler);
                },
                [this](const vector3& incident_direction,
                       const vector3& outgoing_direction)
                {
                    return phase_function_.Evaluate(incident_direction, outgoing_direction);
                },
                [this](const vector3& incident_direction,
                       const vector3& outgoing_direction)
                {
                    return phase_function_.EvaluatePdf(incident_direction, outgoing_direction);
                }
            };
        }
    };
}
