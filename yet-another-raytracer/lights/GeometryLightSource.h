#pragma once

#include "LightSource.h"
#include "math/discrete_distribution.hpp"
#include "Types.h"
#include "objects/GeometryObject.h"

namespace lights
{
    class GeometryLightSource : public LightSource
    {
    public:
        explicit GeometryLightSource(const ObjectCollection& objects);

        void DoWithDistribution(
            const LightingContext& context,
            math::Sampler<space_real>& sampler,
            const distibution_func& job) const noexcept override;

        void DoWithDistribution(const vector3& point, math::Sampler<space_real>& sampler,
                                const distibution_func& job) const noexcept override;

        color_real GetApproximateTotalPower() const noexcept override;

        void DoWithDistributionImpl(const vector3& point,
                                                         const objects::GeometryObject* illuminated_object,
                                                         math::Sampler<space_real>& sampler,
                                                         const distibution_func& job) const noexcept;

        [[nodiscard]] space_real EvaluatePdfExperimental(const ray3& ray) const noexcept override
        {
            // Not implemented
            assert(false);
            std::terminate();
        }

    private:
        math::discrete_distribution<const objects::GeometryObject*, color_real> _distribution;
        std::vector<const objects::GeometryObject*> _objects;
        color_real _totalPower;
    };
}