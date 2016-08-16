#pragma once

#include "LightSource.h"
#include "Types.h"

class DirectionalLightSource : public LightSource
{
public:
	DirectionalLightSource(void);
	~DirectionalLightSource(void);

	const vector3 & direction() const { return m_direction; }
	void direction(const vector3 & val) { m_direction = val; }

	const color_rgbx & color() const { return m_color; }
	void color(const color_rgbx & val) { m_color = val; }

	void DoWithDistribution(const LightingContext & context, math::UniformRandomBitGenerator<unsigned> & randomEngine, const distibution_func & job) const override;
private:
	vector3 m_direction;
	color_rgbx m_color;
};

