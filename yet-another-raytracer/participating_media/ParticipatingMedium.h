#pragma once

#include "media_common.h"

#include "math/vector.hpp"
#include "math/random_sample.hpp"

#include "Types.h"

namespace participating_media
{
    using scatter_generator_t = std::function<scattering_event(const vector3& incident_direction,
                                                               std::size_t color_index,
                                                               math::Sampler<space_real>& sampler)>;

    using phase_function_t = std::function<spectral_coeffs(const vector3& incident_direction,
                                                           const vector3& outgoing_direction,
                                                           std::size_t color_index)>;


    using pdf_evaluator_t = std::function<color_real(
        const vector3& incident_direction,
        const vector3& outgoing_direction,
        std::size_t color_index)>;

    struct medium_properties
    {
        optical_thickness_t absorption;
        optical_thickness_t scattering;
        spectral_coeffs emission;
        scatter_generator_t scatter_generator;
        phase_function_t phase_function;
        pdf_evaluator_t evaluate_pdf;
    };

    struct majorant_sample_result {
        optical_thickness_t majorant;
        space_real valid_distance;
    };

    struct single_majorant_sample_result {
        color_real majorant;
        space_real valid_distance;
    };

    class ParticipatingMedium
    {
    public:
        virtual ~ParticipatingMedium() = default;

        // Returns majorant extinction along ray within max_distance. 0 <= max_distance <= +inf
        [[nodiscard]] virtual majorant_sample_result SampleMajorantExtinction(
            const ray3& ray,
            space_real max_distance) const = 0;

        [[nodiscard]] virtual single_majorant_sample_result SampleMajorantExtinction(
            const ray3& ray,
            space_real max_distance,
            const std::size_t color_index
            ) const = 0;

        [[nodiscard]] virtual medium_properties SampleProperties(const vector3& point) const = 0;

        // [[nodiscard]] virtual spectral_coeffs EvaluatePhaseFunction(
        //     const vector3& point,
        //     const vector3& incident_direction,
        //     const vector3& outgoing_direction) const = 0;

        // // Generates random direction for scattering
        // [[nodiscard]] virtual scattering_sample SampleScattering(const ray3& ray) const = 0;
    };

    constexpr color_real thickness_to_scalar(const optical_thickness_t& thickness)
    {
        return math::max_element(thickness);
    }
}
