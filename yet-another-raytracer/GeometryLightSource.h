#pragma once

#include "LightSource.h"
#include "discrete_distribution.hpp"

class GeometryObject;

class GeometryLightSource : public LightSource
{
public:
	GeometryLightSource(const std::vector<GeometryObject*> & objects);

	void IterateOverFluxes(const LightingContext & context, const RayEvaluator & rayEvaluator, math::UniformRandomBitGenerator<unsigned int> & randomEngine, const flux_func & job) const override;
private:
	math::discrete_distribution<GeometryObject *, color_real> _distribution;

};
