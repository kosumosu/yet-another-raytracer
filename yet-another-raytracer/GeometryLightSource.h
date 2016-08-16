#pragma once

#include "LightSource.h"
#include "discrete_distribution.hpp"
#include "Types.h"
#include "GeometryObject.h"

class GeometryObject;

class GeometryLightSource : public LightSource
{
public:
	GeometryLightSource(const ObjectCollection & objects, size_t sampleCount);

	void DoWithDistribution(const LightingContext & context, math::UniformRandomBitGenerator<unsigned> & randomEngine, const distibution_func & job) const override;
private:
	math::discrete_distribution<GeometryObject *, color_real> _distribution;
	std::vector<GeometryObject *> _objects;
	size_t _sampleCount;
	color_real _totalPower;
};
