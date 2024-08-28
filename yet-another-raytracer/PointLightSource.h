#pragma once

#include "LightSource.h"
#include "Attenuation.h"
#include "Types.h"

class PointLightSource : public LightSource
{
public:
	PointLightSource();

	const vector3 & position() const { return m_position; }
	void position(const vector3 & val) { m_position = val; }

	const color_rgb & color() const { return m_color; }
	void color(const color_rgb & val) { m_color = val; }

	Attenuation attenuation() const { return m_attenuation; }
	void attenuation(Attenuation val) { m_attenuation = val; }

	void DoWithDistribution(const LightingContext & context, math::Sampler<space_real> & sampler, const distibution_func & job) const override;

	color_real GetApproximateTotalPower() const override;
private:
	vector3 m_position;
	color_rgb m_color;
	Attenuation m_attenuation;
};

