#pragma once

#include "LightingCommon.h"
#include "Types.h"
#include <vector>
#include <memory>

class LightingContext;
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

	virtual void IterateOverFluxes(const LightingContext & context, const RayEvaluator & rayEvaluator, const flux_func & job) const = 0;
};

typedef std::vector< std::shared_ptr<LightSource> > LightSourceCollection;