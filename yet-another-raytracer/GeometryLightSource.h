#pragma once

#include "LightSource.h"
#include "math/discrete_distribution.hpp"
#include "Types.h"
#include "objects/GeometryObject.h"


class GeometryLightSource : public LightSource
{
public:
	GeometryLightSource(const ObjectCollection& objects);

	void DoWithDistribution(
		const LightingContext& context,
		math::Sampler<space_real>& sampler,
		const distibution_func& job) const override;

	color_real GetApproximateTotalPower() const override;
private:
	math::discrete_distribution<const objects::GeometryObject*, color_real> _distribution;
	std::vector<const objects::GeometryObject*> _objects;
	color_real _totalPower;
};
