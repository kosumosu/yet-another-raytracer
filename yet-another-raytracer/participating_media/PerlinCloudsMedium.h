#pragma once

#include "ParticipatingMedium.h"
#include "PhaseFunction.h"
#include "Sampler.h"


#include <PerlinNoise.hpp>


namespace participating_media
{
    template <class TFunc>
    concept CDensityMultiplier = std::invocable<TFunc, const vector3&>;

    template <CPhaseFunction TPhaseFunction, CDensityMultiplier TDensityMultiplier>
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

        const color_real sumScale_;

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
              , phase_function_(phase_function)
              , density_multiplier_(std::move(density_multiplier))
              , noise_(seed)
              , sumScale_(color_real(1.0) / sumOfOctaves(multiplier, octaves_))
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
            const auto density = evaluateDensity(point);

            return {
                absorption_ * density,
                scattering_ * density,
                emission_,
                [this](const vector3& incident_direction,
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

    private:
        [[nodiscard]]
        color_real evaluateDensity(const vector3& point) const {
            const auto noise = evaluateNoise(point);
            return noise * density_multiplier_(point);
        }

        [[nodiscard]]
        color_real evaluateNoise(const vector3& point) const {
            const auto scaled_pos = point * inv_size_;

            auto final_pos = scaled_pos;

            color_real sum = 0.0;
            color_real amplitude = 1.0;
            for (std::size_t i = 0; i < octaves_; ++i) {
                sum += amplitude * noise_.noise3D(final_pos[0], final_pos[1], final_pos[2]);
                amplitude *= multiplier_;
                final_pos = final_pos * 2;
            }
            return std::max(color_real(0.0), color_real(sum) * sumScale_);
        }

        static color_real sumOfOctaves(color_real multiplier, std::size_t octaveCount) {
            const auto sum = (color_real(1.0) - color_real(std::pow(multiplier, octaveCount))) / (color_real(1.0) - multiplier );
            return sum;
        }
    };
}
