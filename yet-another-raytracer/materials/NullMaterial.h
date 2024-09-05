#pragma once

#include "Material.h"


namespace materials
{
    // Fully transmittive meterial.
    class NullMaterial final : public Material
    {
        using bsdf_functional_distribution = FunctionalDistribution<const bsdf_sample, const vector3, space_real>;

    public:
        [[nodiscard]] color_real GetEmissionImportance() const override
        {
            return color_real(0);
        }

        void WithBsdfDistribution(const objects::GeometryObject& object,
                                  const vector3& hitPoint,
                                  const vector3& normal,
                                  const uvs_t& uvs,
                                  const vector3& incidentDirection,
                                  math::Sampler<space_real>& sampler,
                                  const bsdf_distribution_func& job) const override
        {
            job(
                bsdf_functional_distribution(
                    1U,
                    false,
                    // generate delta sample
                    [&]()
                    {
                        const auto cosTheta = -math::dot(incidentDirection, normal);
                        return math::random_sample<const bsdf_sample, space_real>(
                            bsdf_sample(
                                true,
                                true,
                                incidentDirection,
                                [=]()
                                {
                                    return color_rgb::fill(1.0 / std::abs(cosTheta));
                                }),
                            space_real(1.0),
                            true);
                    }));
        }

        [[nodiscard]] color_rgb EvaluateEmission(const objects::GeometryObject& object,
                                                 const vector3& hitPoint,
                                                 const vector3& normal,
                                                 const uvs_t& uvs,
                                                 const vector3& incidentDirection,
                                                 math::Sampler<space_real>& sampler) const override
        {
            return color_rgb::zero();
        }

        [[nodiscard]] color_rgb EvaluateNonDeltaScattering(const objects::GeometryObject& object,
                                                           const vector3& hitPoint,
                                                           const vector3& normal,
                                                           const uvs_t& uvs,
                                                           const vector3& incidentDirection,
                                                           const vector3& outgoingDirection,
                                                           math::Sampler<space_real>& sampler) const override
        {
            // If incidentDirection != outgoingDirection which theoretically is impossible.
            return color_rgb::zero();
        }

        [[nodiscard]] color_rgb EvaluateTransmittance(const objects::GeometryObject& object,
                                                     const vector3& hitPoint,
                                                     const vector3& normal,
                                                     const uvs_t& uvs,
                                                     const vector3& direction,
                                                     math::Sampler<space_real>& sampler) const override
        {
            return color_rgb::one();
        }
    };
}
