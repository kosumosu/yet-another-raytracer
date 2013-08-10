#pragma once

#include "LightSource.h"

class __declspec(align(MM_ALIGNMENT)) SkyLightSource : public LightSource
{
public:
	SkyLightSource(void);
	virtual ~SkyLightSource(void);

	virtual FluxCollection * GetFluxes( const vector3 & point, const vector3 & normal ) const;

	unsigned int samples() const { return m_samples; }
	void samples(unsigned int val)
	{
		m_samples = val;
		m_color_per_sample = m_color / color_real(m_samples);
	}

	const color4 & color() const { return m_color; }
	void color(const color4 & val)
	{
		m_color = val;
		m_color_per_sample = m_color / color_real(m_samples);
	}

private:
	unsigned int m_samples;
	color4 m_color;
	color4 m_color_per_sample;
};

