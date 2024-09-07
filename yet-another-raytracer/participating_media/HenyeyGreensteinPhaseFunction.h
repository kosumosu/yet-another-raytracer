#pragma once
#include "PhaseFunction.h"

#include "math/helper.hpp"

namespace participating_media
{
    class HenyeyGreensteinPhaseFunction final : public PhaseFunction
    {
        const space_real g_; // forward-backward coeff
        const space_real g_squared_;

    public:
        explicit HenyeyGreensteinPhaseFunction(space_real g)
            : g_(g)
              , g_squared_(math::square(g))
        {
        }

        [[nodiscard]] scattering_event Sample(const vector3& incident_direction,
                                              math::Sampler<space_real>& sampler) const override
        {
            // auto uniform_direction = math::sphericalRand<space_real>(sampler);
            // const auto value = Evaluate(incident_direction, uniform_direction);
            //
            // return {
            //     std::move(uniform_direction),
            //     value
            // };


            const auto u = sampler.Get2D();
            space_real cosTheta;
            if (std::abs(g_) < space_real(1e-3))
            {
                cosTheta = space_real(1) - space_real(2) * u[0];
            }
            else
            {
                cosTheta = -space_real(1) / (space_real(2) * g_) *
                (space_real(1) + g_squared_ -
                    math::square((space_real(1) - g_squared_) / (space_real(1) + g_ - space_real(2) * g_ * u[0])));
            }

            const auto sinTheta = math::safeSqrt(space_real(1) - math::square(cosTheta));
            const auto phi = space_real(2) * std::numbers::pi_v<space_real> * u[1];
            const auto wFrame = math::makeCoordSystemFromVectorZ(incident_direction);

            const auto dir = math::from_theta_phi_towards_z(sinTheta, cosTheta, phi);
            const auto outgoing_direction = -math::from_local(wFrame, dir);

            return {
                outgoing_direction,
                Evaluate(incident_direction, outgoing_direction),
                EvaluatePdf(incident_direction, outgoing_direction)
            };
        }

        [[nodiscard]] spectral_coeffs Evaluate(const vector3& incident_direction,
                                               const vector3& outgoing_direction) const override
        {
            return spectral_coeffs::fill(EvaluateScalar(incident_direction, outgoing_direction));
        }

        [[nodiscard]] color_real EvaluatePdf(
            const vector3& incident_direction,
            const vector3& outgoing_direction) const override
        {
            return EvaluateScalar(incident_direction, outgoing_direction);
        }

    private:
        [[nodiscard]] color_real EvaluateScalar(const vector3& incident_direction,
                                                const vector3& outgoing_direction) const
        {
            const auto cosTheta = color_real(-math::dot(incident_direction, outgoing_direction));
            assert(std::abs(cosTheta) <= 1);
            const auto denom = color_real(1) + color_real(g_squared_) + color_real(2) * color_real(g_) * cosTheta;
            const auto result = std::numbers::inv_pi_v<color_real> * color_real(0.25)
                * (color_real(1) - color_real(g_squared_))
                / (denom * math::safeSqrt(denom));
            return result;
        }
    };
}
