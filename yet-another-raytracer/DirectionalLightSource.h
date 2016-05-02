#pragma once

#include "LightSource.h"
#include "Types.h"

class DirectionalLightSource : public LightSource
{
public:
	DirectionalLightSource(void);
	~DirectionalLightSource(void);

	virtual FluxCollection * GetFluxes( const vector3 & point, const vector3 & normal, const RayEvaluator & rayEvaluator, unsigned int depthLeft, space_real bias, bool allowSubdivision) const override;

	const vector3 & direction() const { return m_direction; }
	void direction(const vector3 & val) { m_direction = val; }

	const color_rgbx & color() const { return m_color; }
	void color(const color_rgbx & val) { m_color = val; }


private:
	vector3 m_direction;
	color_rgbx m_color;
};

