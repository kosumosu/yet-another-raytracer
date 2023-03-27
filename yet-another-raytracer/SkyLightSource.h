#pragma once

#include "LightSource.h"
#include "Types.h"

class SkyLightSource : public LightSource
{
public:
	SkyLightSource();

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

	void DoWithDistribution(const LightingContext & context, math::Sampler<space_real> & sampler, const distibution_func & job) const override;


	color_real GetApproximateTotalPower() const override;
private:
	unsigned int m_samples;
	color_rgbx m_color;
};

