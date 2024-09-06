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

            // const auto dotxy = math::dot(wFrame[0], wFrame[1]);
            // const auto dotxz = math::dot(wFrame[0], wFrame[2]);
            // const auto dotyz = math::dot(wFrame[1], wFrame[2]);

            //assert(dotxy == space_real(0) && dotxz == space_real(0) && dotyz == space_real(0));

            const auto dir = math::fromThetaPhi(sinTheta, cosTheta, phi);
            const auto outgoing_direction = math::fromLocal(wFrame, dir);

            // if (math::length(outgoing_direction) > 1.01)
            // {
            //     const auto dir_len = math::length(dir);
            //     const auto outgoing_direction_len = math::length(outgoing_direction);
            //     const auto incident_direction_len = math::length(incident_direction);
            //     int x = 23;
            // }

            // const auto coss = math::dot(incident_direction, outgoing_direction);
            //
            // if (std::abs(coss - cosTheta) > 0.00001)
            // {
            //     int sd = 0;
            // }

            return {
                outgoing_direction,
                spectral_coeffs::one()
            };
        }

        [[nodiscard]] spectral_coeffs Evaluate(const vector3& incident_direction,
                                               const vector3& outgoing_direction) const override
        {
            const auto cosTheta = color_real(-math::dot(incident_direction, outgoing_direction));
            assert(std::abs(cosTheta) <= 1);
            const auto denom = color_real(1) + color_real(g_squared_) + color_real(2) * color_real(g_) * cosTheta;
            const auto result = std::numbers::inv_pi_v<color_real> * color_real(0.25)
                * (color_real(1) - color_real(g_squared_))
                / (denom * math::safeSqrt(denom));
            return spectral_coeffs::fill(result);
        }
    };
}
