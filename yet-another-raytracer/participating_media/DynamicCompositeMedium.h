#pragma once

#include "ParticipatingMedium.h"

#include <limits>
#include <unordered_set>

namespace participating_media
{
    // Not thread-safe!
    class DynamicCompositeMedium final : public ParticipatingMedium
    {
        std::unordered_set<const ParticipatingMedium*> media_;
        mutable std::vector<medium_properties> props_temp_storage_;

    public:
        void CopyMedia(const DynamicCompositeMedium& media)
        {
            media_.clear();
            media_.insert(media.media_.begin(), media.media_.end());
        }

        void PushMedium(const ParticipatingMedium* medium)
        {
            assert(medium != nullptr);
            media_.insert(medium);
        }

        void PopMedium(const ParticipatingMedium* medium)
        {
            assert(medium != nullptr);
            media_.erase(medium);
        }

        void ClearMedia()
        {
            media_.clear();
        }

        [[nodiscard]] majorant_sample_result
        SampleMajorantExtinction(const ray3& ray, space_real max_distance) const override {
            optical_thickness_t majorant_sum = optical_thickness_t::zero();
            space_real min_valid_distance = std::numeric_limits<space_real>::max();
            for (const auto& medium: media_) {
                const auto [majorant, valid_distance] = medium->SampleMajorantExtinction(ray, max_distance);
                majorant_sum += majorant;
                min_valid_distance = std::min(min_valid_distance, valid_distance);
            }

            return {
                majorant_sum,
                min_valid_distance
            };
        }

        [[nodiscard]] single_majorant_sample_result SampleMajorantExtinction(
            const ray3 &ray,
            space_real max_distance,
            const std::size_t color_index
            ) const override {
            color_real majorant_sum = 0.0;
            space_real min_valid_distance = std::numeric_limits<space_real>::max();
            for (const auto& medium: media_) {
                const auto [majorant, valid_distance] = medium->SampleMajorantExtinction(ray, max_distance, color_index);
                majorant_sum += majorant;
                min_valid_distance = std::min(min_valid_distance, valid_distance);
            }

            return {
                majorant_sum,
                min_valid_distance
            };
        }

        [[nodiscard]] medium_properties SampleProperties(const vector3& point) const override
        {
            optical_thickness_t absorption_sum = optical_thickness_t::zero();
            optical_thickness_t scattering_sum = optical_thickness_t::zero();
            spectral_coeffs weighted_emission_sum = spectral_coeffs::zero();

            props_temp_storage_.clear();

            for (const auto& medium : media_)
            {
                auto props = medium->SampleProperties(point);
                absorption_sum += props.absorption;
                scattering_sum += props.scattering;
                weighted_emission_sum += props.emission * props.absorption;
                props_temp_storage_.push_back(std::move(props));
            }

            const spectral_coeffs emission{
                absorption_sum[0] == optical_thickness_scalar_t(0)
                    ? optical_thickness_scalar_t(0)
                    : weighted_emission_sum[0] / absorption_sum[0],
                absorption_sum[1] == optical_thickness_scalar_t(0)
                    ? optical_thickness_scalar_t(0)
                    : weighted_emission_sum[1] / absorption_sum[1],
                absorption_sum[2] == optical_thickness_scalar_t(0)
                    ? optical_thickness_scalar_t(0)
                    : weighted_emission_sum[2] / absorption_sum[2],
            };


            return {
                absorption_sum,
                scattering_sum,
                emission,
                [scattering_sum, this](const vector3& incident_direction,
                std::size_t color_index,
                                                              math::Sampler<space_real>& sampler)
                {
                    //return props_temp_storage_.front().scatter_generator(incident_direction, sampler);


                    const auto scattering_sum_scalar = scattering_sum[color_index];


                    //const auto inv_scattering_sum = optical_thickness_scalar_t(1.0) / scattering_sum;

                    const auto random_threshold = optical_thickness_scalar_t(sampler.Get1D()) * scattering_sum_scalar;
                    // assuming extinction_to_scalar is linear
                    auto weight_sum = optical_thickness_scalar_t(0.0);

                    for (std::size_t i = 0; i + 1 < props_temp_storage_.size(); ++i)
                    {
                        const auto& props = props_temp_storage_[i];
                        const auto weight = props.scattering[color_index];
                        weight_sum += weight;
                        if (random_threshold < weight_sum)
                            return fix_transmittance(
                                props.scatter_generator(incident_direction, color_index, sampler),
                                spectral_coeffs::fill(weight / scattering_sum_scalar), //props.scattering, //* inv_scattering_sum,
                                weight / scattering_sum_scalar);
                        //  (weight / weight_sum) == pdf
                    }
                    const auto& props = props_temp_storage_.back();

                    scattering_event event = fix_transmittance(
                        props.scatter_generator(
                            incident_direction, color_index, sampler),
                            spectral_coeffs::fill((scattering_sum_scalar -
                                weight_sum) / scattering_sum_scalar), //props.scattering, // * inv_scattering_sum,
                        (scattering_sum_scalar -
                            weight_sum) / scattering_sum_scalar);

                    assert(event.pdf != 0);
                    assert(std::isfinite(event.pdf));

                    return event;
                },
                [scattering_sum, this](const vector3& incident_direction,
                                       const vector3& outgoing_direction,
                                       std::size_t color_index)
                {
                    //return props_temp_storage_.front().phase_function(incident_direction, outgoing_direction);

                    spectral_coeffs phase_function_sum = spectral_coeffs::zero();
                    for (const auto& props : props_temp_storage_)
                    {
                        phase_function_sum += props.phase_function(incident_direction, outgoing_direction, color_index) * props.
                            scattering;
                    }
                    return phase_function_sum / scattering_sum;
                },
                [scattering_sum, this](const vector3& incident_direction,
                                              const vector3& outgoing_direction,
                                              std::size_t color_index)
                {
                    //return props_temp_storage_.front().evaluate_pdf(incident_direction, outgoing_direction);

                    auto phase_function_sum = color_real(0);
                    for (const auto& props : props_temp_storage_)
                    {
                        phase_function_sum += props.evaluate_pdf(incident_direction, outgoing_direction, color_index)
                            * props.scattering[color_index];
                    }
                    return phase_function_sum / scattering_sum[color_index];
                }
            };
        }

        static scattering_event fix_transmittance(
            const scattering_event& event,
            const spectral_coeffs& scale,
            const color_real& pdf_scale)
        {
            return {
                event.direction,
                event.transmittance * scale,
                event.pdf * pdf_scale
            };
        }
    };
}
