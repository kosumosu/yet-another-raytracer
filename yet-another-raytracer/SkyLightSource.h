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

	const color_rgb & color() const { return m_color; }
	void color(const color_rgb & val)
	{
		m_color = val;
	}

	void DoWithDistribution(const LightingContext & context, math::Sampler<space_real> & sampler, const distibution_func & job) const override;

	void DoWithDistribution(const vector3& point, math::Sampler<space_real>& sampler,
		const distibution_func& job) const override;

	color_real GetApproximateTotalPower() const override;
private:
	unsigned int m_samples;
	color_rgb m_color;
};

