#pragma once

#include "PhaseFunction.h"

namespace participating_media
{
    class RayleighPhaseFunction final : public PhaseFunction
    {
        constexpr static bool USE_DIRECT_SAMPLING = true;

    public:
        [[nodiscard]] scattering_event Sample(const vector3& incident_direction,
                                              math::Sampler<space_real>& sampler) const override
        {
            if constexpr (USE_DIRECT_SAMPLING)
            {
                return SampleDirectly(incident_direction, sampler);
            }
            else
            {
                return SampleUsingUniformSphereDistribution(incident_direction, sampler);
            }
        }

        [[nodiscard]] spectral_coeffs Evaluate(const vector3& incident_direction,
                                               const vector3& outgoing_direction) const override
        {
            return spectral_coeffs::fill(EvaluateScalar(incident_direction, outgoing_direction));
        }

        [[nodiscard]] color_real EvaluatePdf(const vector3& incident_direction,
                                             const vector3& outgoing_direction) const override
        {
            if constexpr (USE_DIRECT_SAMPLING)
            {
                return EvaluateScalar(incident_direction, outgoing_direction);
            }
            else
            {
                return space_real(0.25) * std::numbers::inv_pi_v<space_real>;
            }
        }

    private:
        [[nodiscard]] static color_real EvaluateScalar(const vector3& incident_direction,
                                                       const vector3& outgoing_direction)
        {
            const auto cosTheta = math::dot(incident_direction, outgoing_direction);
            return
                color_real(0.25) * std::numbers::inv_pi_v<color_real>
                * color_real(0.75) * (1 + math::square(color_real(cosTheta)));
        }


        scattering_event SampleUsingUniformSphereDistribution(const vector3& incident_direction,
                                                              math::Sampler<space_real>& sampler) const
        {
            auto direction = math::sphericalRand<space_real>(sampler);
            auto transmittance = Evaluate(incident_direction, direction);
            auto pdf = EvaluatePdf(incident_direction, direction);

            return scattering_event{
                std::move(direction),
                std::move(transmittance),
                pdf
            };
        }

        [[nodiscard]] scattering_event SampleDirectly(const vector3& incident_direction,
                                                      math::Sampler<space_real>& sampler) const
        {
            const auto v = sampler.Get2D();

            const auto a = 2 * v[0] - 1;
            const auto u = -std::pow(2 * a + std::sqrt(4 * math::square(a) + 1), 1.0 / 3.0);
            const auto cosTheta = u - 1 / u;

            const auto sinTheta = math::safeSqrt(space_real(1) - math::square(cosTheta));
            const auto phi = space_real(2) * std::numbers::pi_v<space_real> * v[1];
            const auto wFrame = math::makeCoordSystemFromVectorZ(incident_direction);

            const auto dir = math::from_theta_phi_towards_z(sinTheta, cosTheta, phi);
            const auto outgoing_direction = -math::from_local(wFrame, dir);

            //const auto cosCheck = math::dot(incident_direction, outgoing_direction);

            return {
                outgoing_direction,
                Evaluate(incident_direction, outgoing_direction),
                EvaluatePdf(incident_direction, outgoing_direction)
            };
        }
    };
}
