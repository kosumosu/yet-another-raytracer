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
            optical_thickness_t absorption = optical_thickness_t::zero();
            optical_thickness_t scattering = optical_thickness_t::zero();
            spectral_coeffs emission = spectral_coeffs::zero();

            props_temp_storage_.clear();

            for (const auto& medium : media_)
            {
                auto props = medium->SampleProperties(point);
                absorption += props.absorption;
                scattering += props.scattering;
                emission += props.emission;
                props_temp_storage_.push_back(std::move(props));
            }

            return {
                absorption,
                scattering,
                emission,
                [scattering, this](math::Sampler<space_real>& sampler,
                                   const extinction_to_scalar_t& extinction_to_scalar)
                {
                    const auto random_threshold = sampler.Get1D() * extinction_to_scalar(scattering);
                    // assuming extinction_to_scalar is linear
                    auto sum = space_real(0.0);

                    for (std::size_t i = 0; i + 1 < props_temp_storage_.size(); ++i)
                    {
                        auto& props = props_temp_storage_[i];
                        sum += extinction_to_scalar(props.absorption);
                        if (sum < random_threshold)
                            return props.scatter_generator(sampler, extinction_to_scalar);
                    }
                    return props_temp_storage_.back().scatter_generator(sampler, extinction_to_scalar);
                },
                [scattering, this](const vector3& incident_direction,
                                          const vector3& outgoing_direction)
                {
                    spectral_coeffs phase_function_sum = spectral_coeffs::zero();
                    for (const auto& props : props_temp_storage_)
                    {
                        phase_function_sum += props.phase_function(incident_direction, outgoing_direction) * props.
                            scattering;
                    }
                    return phase_function_sum / scattering;
                }
            };
        }
    };
}
