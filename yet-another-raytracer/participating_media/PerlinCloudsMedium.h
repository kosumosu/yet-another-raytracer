#pragma once

#include "ParticipatingMedium.h"
#include "PhaseFunction.h"
#include "Sampler.h"


#include <PerlinNoise.hpp>

#include <limits>

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
        const color_real coverage_shift_;
        const color_real coverage_factor_;
        const size_t octaves_;
        const size_t curl_octaves_;
        const color_real multiplier_;
        const color_real curl_multiplier_;

        const TPhaseFunction& phase_function_;
        TDensityMultiplier density_multiplier_;

        const siv::PerlinNoise noise_;

        const color_real sum_scale_;
        const color_real curlSumScale_;

    public:
        PerlinCloudsMedium(
            optical_thickness_t absorption,
            optical_thickness_t scattering,
            spectral_coeffs emission,
            const space_real& size,
            color_real coverage,
            size_t octaves,
            color_real multiplier,
            TDensityMultiplier density_multiplier,
            std::size_t seed,
            const TPhaseFunction& phase_function)
            : absorption_(std::move(absorption))
              , scattering_(std::move(scattering))
              , majorant_(absorption_ + scattering_)
              , emission_(std::move(emission))
              , inv_size_(1.0 / size)
              , coverage_shift_(1 - coverage * 2)
              , coverage_factor_(1 / (1 - coverage_shift_))
              , octaves_(octaves)
              , curl_octaves_(octaves)
              , multiplier_(multiplier)
              , curl_multiplier_(0.5)
              , phase_function_(phase_function)
              , density_multiplier_(std::move(density_multiplier))
              , noise_(seed)
              , sum_scale_(color_real(1.0) / sumOfOctaves(multiplier, octaves_))
              , curlSumScale_(color_real(1.0) / sumOfOctaves(multiplier, curl_octaves_))
        {
        }

        [[nodiscard]] majorant_sample_result SampleMajorantExtinction(
            const ray3& ray,
            space_real max_distance) const override {
            return {
                majorant_,
                std::numeric_limits<space_real>::max()
                };
        }

        [[nodiscard]] single_majorant_sample_result SampleMajorantExtinction(const ray3 &ray, space_real max_distance,
            const std::size_t color_index) const override {
            return {
                majorant_[color_index],
                std::numeric_limits<space_real>::max()
                };
        }

        [[nodiscard]] medium_properties SampleProperties(const vector3& point) const override
        {
            const auto relative_density = evaluateDensity(point);

            // const auto emission_density = std::max(color_real(relative_density) - 0.1f, color_real(0.0));
            // const auto scatter_density = color_real(1) - emission_density;

            return {
                // absorption_ * relative_density * emission_density,
                // scattering_ * relative_density * scatter_density,
                absorption_ * relative_density,
                scattering_ * relative_density,
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

            const auto curl_offset = vector3(
                evaluateRawNoise(scaled_pos, curl_octaves_, 0.5),
                evaluateRawNoise(scaled_pos + vector3(137.0, 0.0, 0.0), curl_octaves_, 0.5),
                evaluateRawNoise(scaled_pos + vector3(0.0, 193.0, 0.0), curl_octaves_, 0.5)
                );

            const auto final_pos = scaled_pos + curl_offset * color_real(0.5);

            //const auto final_pos = scaled_pos ;

            const auto noise_value = evaluateRawNoise(final_pos, octaves_, multiplier_);
            const auto final_value = (noise_value  * sum_scale_ - coverage_shift_) * coverage_factor_;
            return math::saturate(final_value, color_real(0.0), color_real(1.0));
        }

        [[nodiscard]]
        color_real evaluateRawNoise(const vector3& point, std::size_t octaves, color_real multiplier) const {
            auto final_pos = point;

            color_real sum = 0.0;
            color_real amplitude = 1.0;
            for (std::size_t i = 0; i < octaves; ++i) {
                // sum += amplitude * std::abs(noise_.noise3D(final_pos[0], final_pos[1], final_pos[2]));
                sum += amplitude * std::abs(noise_.noise3D_Branchless(final_pos[0], final_pos[1], final_pos[2]));
                amplitude *= multiplier;
                final_pos = final_pos * 2;
            }
            return std::max(color_real(0.0), color_real(sum));
        }

        static color_real sumOfOctaves(color_real multiplier, std::size_t octaveCount) {
            const auto sum = (color_real(1.0) - color_real(std::pow(multiplier, octaveCount))) / (color_real(1.0) - multiplier );
            return sum;
        }
    };
}
