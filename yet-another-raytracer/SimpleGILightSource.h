#pragma once

#include "LightSource.h"
#include "Types.h"

class SimpleGILightSource : public LightSource
{
public:
	SimpleGILightSource(void);
	virtual ~SimpleGILightSource(void);

	unsigned int samples() const { return m_samples; }
	void samples(unsigned int val)
	{
		m_samples = val;
	}


	void IterateOverFluxes(const LightingContext & context, const RayEvaluator & rayEvaluator, math::UniformRandomBitGenerator<unsigned int> & randomEngine, const flux_func & job) const override;
private:
	unsigned int m_samples;
};

