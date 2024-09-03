#pragma once

#include "Sampler.h"
#include "Distribution.h"
#include "Types.h"
#include <functional>
#include <utility>

namespace objects
{
    class GeometryObject;
}


namespace materials
{
    struct bsdf_sample
    {
        bsdf_sample(bool isTransition, vector3 outgoingDirection, std::function<color_rgb()> evaluate)
            : isTransition(isTransition)
              , outgoingDirection(std::move(outgoingDirection))
              , evaluate(std::move(evaluate))
        {
        }

        bool isTransition;
        vector3 outgoingDirection;
        std::function<color_rgb()> evaluate;
    };

    using bsdf_distribution = Distribution<const bsdf_sample, const vector3, space_real>;
    using bsdf_distribution_func = std::function<void(const bsdf_distribution& distribution)>;

    class Material
    {
    public:
        virtual ~Material() = default;

        // first integrate over hemisphere, then find average across everywhere
        [[nodiscard]] virtual color_real GetEmissionImportance() const = 0;

        virtual void WithBsdfDistribution(
            const objects::GeometryObject& object,
            const vector3& hitPoint,
            const vector3& normal,
            const uvs_t& uvs,
            const vector3& incidentDirection,
            math::Sampler<space_real>& sampler,
            const bsdf_distribution_func& job) const = 0;

        [[nodiscard]] virtual color_rgb EvaluateEmission(
            const objects::GeometryObject& object,
            const vector3& hitPoint,
            const vector3& normal,
            const uvs_t& uvs,
            const vector3& incidentDirection,
            math::Sampler<space_real>& sampler) const = 0;

        [[nodiscard]] virtual color_rgb EvaluateNonDeltaScattering(
            const objects::GeometryObject& object,
            const vector3& hitPoint,
            const vector3& normal,
            const uvs_t& uvs,
            const vector3& incidentDirection,
            const vector3& outgoingDirection,
            math::Sampler<space_real>& sampler) const = 0;

        [[nodiscard]] virtual color_rgb EvaluateTransmittance(
            const objects::GeometryObject& object,
            const vector3& hitPoint,
            const vector3& normal,
            const uvs_t& uvs,
            const vector3& direction,
            math::Sampler<space_real>& sampler) const = 0;
    };
}
