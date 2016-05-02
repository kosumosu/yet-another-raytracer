#pragma once

#include "LightSource.h"

class SimpleGILightSource : public LightSource
{
public:
	SimpleGILightSource(void);
	virtual ~SimpleGILightSource(void);

	virtual FluxCollection * GetFluxes(const vector3 & point, const vector3 & normal, const RayEvaluator & rayEvaluator, unsigned int depthLeft, space_real bias, bool allowSubdivision) const override;

	unsigned int samples() const { return m_samples; }
	void samples(unsigned int val)
	{
		m_samples = val;
	}


private:
	unsigned int m_samples;
};

