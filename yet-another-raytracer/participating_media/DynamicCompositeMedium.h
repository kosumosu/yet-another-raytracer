#pragma once

#include "ParticipatingMedium.h"

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

        [[nodiscard]] optical_thickness_t SampleMajorantExtinction(const ray3& ray, space_real max_distance) const override
        {
            return std::accumulate(
                media_.cbegin(),
                media_.cend(),
                optical_thickness_t::zero(),
                [&](auto acc, const auto& medium) { return acc + medium->SampleMajorantExtinction(ray, max_distance); }
            );
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

            const spectral_coeffs emission {
                absorption_sum[0] == optical_thickness_scalar_t(0) ? optical_thickness_scalar_t(0) : weighted_emission_sum[0] / absorption_sum[0],
                absorption_sum[1] == optical_thickness_scalar_t(0) ? optical_thickness_scalar_t(0) : weighted_emission_sum[1] / absorption_sum[1],
                absorption_sum[2] == optical_thickness_scalar_t(0) ? optical_thickness_scalar_t(0) : weighted_emission_sum[2] / absorption_sum[2],
            };


            return {
                absorption_sum,
                scattering_sum,
                emission,
                [scattering_sum, this](const vector3& incident_direction, math::Sampler<space_real>& sampler)
                {
                    const auto inv_scattering_sum = optical_thickness_scalar_t(1.0) / scattering_sum;

                    const auto scattering_sum_scalar = color::get_importance(scattering_sum);
                    const auto random_threshold = optical_thickness_scalar_t(sampler.Get1D()) * scattering_sum_scalar;
                    // assuming extinction_to_scalar is linear
                    auto weight_sum = optical_thickness_scalar_t(0.0);

                    for (std::size_t i = 0; i + 1 < props_temp_storage_.size(); ++i)
                    {
                        const auto& props = props_temp_storage_[i];
                        const auto weight = color::get_importance(props.scattering);
                        weight_sum += weight;
                        if (random_threshold < weight_sum)
                            return scale_transmittance(props.scatter_generator(incident_direction, sampler), props.scattering * inv_scattering_sum * weight_sum / weight); //  (weight / weight_sum) == pdf

                    }
                    const auto& props = props_temp_storage_.back();
                    return scale_transmittance(props.scatter_generator(incident_direction, sampler), props.scattering * weight_sum / (optical_thickness_scalar_t(1.0) - weight_sum));
                },
                [scattering_sum, this](const vector3& incident_direction,
                                          const vector3& outgoing_direction)
                {
                    spectral_coeffs phase_function_sum = spectral_coeffs::zero();
                    for (const auto& props : props_temp_storage_)
                    {
                        phase_function_sum += props.phase_function(incident_direction, outgoing_direction) * props.
                            scattering;
                    }
                    return phase_function_sum / scattering_sum;
                }
            };
        }

        static scattering_event scale_transmittance(const scattering_event& event, const spectral_coeffs& scale)
        {
            return {
                event.direction,
                event.transmittance * scale
            };
        }
    };
}
