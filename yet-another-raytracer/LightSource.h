#pragma once

#include "LightingCommon.h"
#include "Types.h"
#include <vector>
#include <memory>

class RayEvaluator;

class LightSource
{
public:

	LightSource(void)
	{
	}

	virtual ~LightSource(void)
	{
	}

	virtual void IterateOverFluxes(const vector3 & point, const vector3 & normal, const RayEvaluator & rayEvaluator, unsigned int depthLeft, space_real bias, bool allowSubdivision, const flux_func & job) const = 0;
};

typedef std::vector< std::shared_ptr<LightSource> > LightSourceCollection;