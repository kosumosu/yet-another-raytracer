#pragma once
#include "PhaseFunction.h"

#include "math/helper.hpp"

namespace participating_media
{
    class SunAwareRayleighPhaseFunction final : public PhaseFunction
    {
        const space_real sun_g_; // for second strategy towards sun
        const space_real prob_sun_;
        const vector3 sun_direction_;

    public:
        explicit SunAwareRayleighPhaseFunction(
            space_real sun_g,
            space_real prob_sun,
            vector3 sun_direction
        )
            : sun_g_(sun_g)
              , prob_sun_(prob_sun)
              , sun_direction_(std::move(sun_direction))
        {
        }

        [[nodiscard]] scattering_event Sample(const vector3& incident_direction,
                                              math::Sampler<space_real>& sampler) const override
        {
            const auto strategy_random_value = sampler.Get1D();
            const bool use_sun_strategy = strategy_random_value < prob_sun_;

            auto [outgoing_direction, strategy_prob] =
                use_sun_strategy
                    ? std::make_pair(SampleHGDirection(sun_direction_, sampler, sun_g_), prob_sun_)
                    : std::make_pair(SampleRayleighDirection(incident_direction, sampler), 1 - prob_sun_);

            const auto value = EvaluateRayleighScalar(incident_direction, outgoing_direction);
            const auto pdf1 = value;
            const auto pdf2 = EvaluateHGScalar(sun_direction_, outgoing_direction, color_real(sun_g_));

            const auto denominator = strategy_prob * (pdf1 + pdf2);

            return {
                std::move(outgoing_direction),
                spectral_coeffs::fill(value),
                color_real(denominator)
            };
        }

        [[nodiscard]] spectral_coeffs Evaluate(const vector3& incident_direction,
                                               const vector3& outgoing_direction) const override
        {
            return spectral_coeffs::fill(EvaluateRayleighScalar(incident_direction, outgoing_direction));
        }

        [[nodiscard]] color_real EvaluatePdf(
            const vector3& incident_direction,
            const vector3& outgoing_direction) const override
        {
            // Not implemented
            assert(false);
            std::terminate();
        }

    private:
        [[nodiscard]] static color_real EvaluateHGScalar(
            const vector3& incident_direction,
            const vector3& outgoing_direction,
            const color_real g
        )
        {
            const auto cosTheta = color_real(math::dot(incident_direction, outgoing_direction));
            return EvaluateHGScalar(cosTheta, g);
        }

        [[nodiscard]] static color_real EvaluateHGScalar(
            space_real cosTheta,
            const color_real g
        )
        {
            const auto g_sqr = math::square(g);
            const auto cosThetaRev = -cosTheta;
            assert(std::abs(cosThetaRev) <= 1);
            const auto denom = color_real(1) + g_sqr + color_real(2) * g * cosThetaRev;
            const auto result = std::numbers::inv_pi_v<color_real> * color_real(0.25)
                * (color_real(1) - g_sqr)
                / (denom * math::safeSqrt(denom));
            return result;
        }


        [[nodiscard]] static vector3 SampleHGDirection(const vector3& incident_direction,
                                                       math::Sampler<space_real>& sampler, space_real g)
        {
            const auto sampling_g_sqr = math::square(g);

            const auto u = sampler.Get2D();
            space_real cosTheta;
            if (std::abs(g) < space_real(1e-3))
            {
                cosTheta = space_real(1) - space_real(2) * u[0];
            }
            else
            {
                cosTheta = -space_real(1) / (space_real(2) * g) *
                (space_real(1) + sampling_g_sqr -
                    math::square(
                        (space_real(1) - sampling_g_sqr) / (space_real(1) + g - space_real(2) * g *
                            u[
                                0])));
            }

            const auto sinTheta = math::safeSqrt(space_real(1) - math::square(cosTheta));
            const auto phi = space_real(2) * std::numbers::pi_v<space_real> * u[1];
            const auto dir = math::from_theta_phi_towards_z(sinTheta, cosTheta, phi);

            const auto wFrame = math::makeCoordSystemFromVectorZ(incident_direction);
            const auto outgoing_direction = -math::from_local(wFrame, dir);

            return outgoing_direction;
        }


        [[nodiscard]] static color_real EvaluateRayleighScalar(const vector3& incident_direction,
                                                               const vector3& outgoing_direction)
        {
            const auto cosTheta = math::dot(incident_direction, outgoing_direction);
            return
                color_real(0.25) * std::numbers::inv_pi_v<color_real>
                * color_real(0.75) * (1 + math::square(color_real(cosTheta)));
        }

        [[nodiscard]] vector3 SampleRayleighDirection(const vector3& incident_direction,
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

            return outgoing_direction;
        }
    };
}
