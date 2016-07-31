#pragma once

#include "LightSource.h"
#include "Types.h"

class SimpleGILightSource : public LightSource
{
public:
	SimpleGILightSource(bool includeEmission, size_t samples);
	virtual ~SimpleGILightSource(void);

	void IterateOverFluxes(const LightingContext & context, const RayEvaluator & rayEvaluator, math::UniformRandomBitGenerator<unsigned int> & randomEngine, const flux_func & job) const override;
private:
	size_t _samples;
	bool _includeEmission;
};

