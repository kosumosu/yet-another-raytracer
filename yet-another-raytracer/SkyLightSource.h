#pragma once

#include "LightSource.h"
#include "Types.h"

class SkyLightSource : public LightSource
{
public:
	SkyLightSource(void);
	virtual ~SkyLightSource(void);

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

	void DoWithDistribution(const LightingContext & context, math::UniformRandomBitGenerator<unsigned> & randomEngine, const distibution_func & job) const override;
private:
	unsigned int m_samples;
	color_rgbx m_color;
};

