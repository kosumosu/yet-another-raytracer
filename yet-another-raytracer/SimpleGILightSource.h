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


	void IterateOverFluxes(const vector3 & point, const vector3 & normal, const RayEvaluator & rayEvaluator, unsigned depthLeft, space_real bias, bool allowSubdivision, const flux_func & job) const override;
private:
	unsigned int m_samples;
};

