#pragma once

#include "math/vector.hpp"
#include "math/random_sample.hpp"

#include "Types.h"

namespace participating_media
{
    using optical_thickness_t = color_rgb; //math::vector<space_real, 3>;
    using spectral_coeffs = color_rgb; // math::vector<space_real, 3>;

    struct scattering_event
    {
        vector3 direction;
        spectral_coeffs transmittance;
    };

    //using scattering_sample = math::random_sample<scattering_event, space_real>;

    using extinction_to_scalar_t = std::function<color_real(optical_thickness_t)>;

    using scatter_generator_t = std::function<scattering_event(math::Sampler<space_real>& sampler,
                                                               const extinction_to_scalar_t& extinction_to_scalar)>;

    using phase_function_t = std::function<spectral_coeffs(const vector3& incident_direction,
                                                           const vector3& outgoing_direction)>;
    // extinction_to_scalar must be linear! I.e. sum(f(x)) == f(sum(x))

    struct medium_properties
    {
        optical_thickness_t absorption;
        optical_thickness_t scattering;
        spectral_coeffs emission;
        scatter_generator_t scatter_generator;
        phase_function_t phase_function;
    };

    class ParticipatingMedium
    {
    public:
        virtual ~ParticipatingMedium() = default;

        // Returns majorant extinction along ray within max_distance. 0 <= max_distance <= +inf
        [[nodiscard]] virtual optical_thickness_t SampleMajorantExtinction(
            const ray3& ray,
            space_real max_distance) const = 0;

        [[nodiscard]] virtual medium_properties SampleProperties(const vector3& point) const = 0;

        // [[nodiscard]] virtual spectral_coeffs EvaluatePhaseFunction(
        //     const vector3& point,
        //     const vector3& incident_direction,
        //     const vector3& outgoing_direction) const = 0;

        // // Generates random direction for scattering
        // [[nodiscard]] virtual scattering_sample SampleScattering(const ray3& ray) const = 0;
    };
}
