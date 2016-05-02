#pragma once

#include "Flux.h"
#include <vector>
#include <memory>
#include <vector>

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

	// Gets atoms for the point being lit. Caller must destroy the result.
	virtual FluxCollection * GetFluxes(const vector3 & point, const vector3 & normal, const RayEvaluator & rayEvaluator, unsigned int depthLeft, space_real bias, bool allowSubdivision) const = 0;

};

typedef std::vector< std::shared_ptr<LightSource> > LightSourceCollection;