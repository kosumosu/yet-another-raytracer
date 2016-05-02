#pragma once

#include "LightSource.h"

class __declspec(align(MM_ALIGNMENT)) SkyLightSource : public LightSource
{
public:
	SkyLightSource(void);
	virtual ~SkyLightSource(void);

	virtual FluxCollection * GetFluxes( const vector3 & point, const vector3 & normal, const RayEvaluator & rayEvaluator, unsigned int depthLeft, space_real bias, bool allowSubdivision) const;

	unsigned int samples() const { return m_samples; }
	void samples(unsigned int val)
	{
		m_samples = val;
	}

	const color_rgbx & color() const { return m_color; }
	void color(const color_rgbx & val)
	{
		m_color = val;
	}

private:
	unsigned int m_samples;
	color_rgbx m_color;
};

