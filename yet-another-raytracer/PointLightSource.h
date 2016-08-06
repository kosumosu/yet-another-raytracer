#pragma once

#include "LightSource.h"
#include "Attenuation.h"
#include "Types.h"

class PointLightSource : public LightSource
{
public:
	PointLightSource(void);
	~PointLightSource(void);

	const vector3 & position() const { return m_position; }
	void position(const vector3 & val) { m_position = val; }

	const color_rgbx & color() const { return m_color; }
	void color(const color_rgbx & val) { m_color = val; }

	Attenuation attenuation() const { return m_attenuation; }
	void attenuation(Attenuation val) { m_attenuation = val; }

	void IterateOverFluxes(const LightingContext & context, const RayEvaluator & rayEvaluator, math::UniformRandomBitGenerator<unsigned int> & randomEngine, const flux_func & job) const override;
	void DoWithDistribution(const LightingContext & context, math::UniformRandomBitGenerator<unsigned> & randomEngine, const distibution_func & job) const override;
private:
	vector3 m_position;
	color_rgbx m_color;
	Attenuation m_attenuation;
};

