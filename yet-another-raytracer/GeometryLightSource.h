#pragma once

#include "LightSource.h"
#include "discrete_distribution.hpp"
#include "Types.h"
#include "GeometryObject.h"

class GeometryObject;

class GeometryLightSource : public LightSource
{
public:
	GeometryLightSource(const ObjectCollection & objects);

	void DoWithDistribution(const LightingContext & context, math::UniformRandomBitGenerator<random_int_t> & randomEngine, const distibution_func & job) const override;

	color_real GetApproximateTotalPower() const override;
private:
	math::discrete_distribution<GeometryObject *, color_real> _distribution;
	std::vector<GeometryObject *> _objects;
	color_real _totalPower;
};
