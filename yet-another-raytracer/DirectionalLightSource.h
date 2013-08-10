#pragma once

#include "LightSource.h"
#include "Types.h"

class __declspec(align(MM_ALIGNMENT)) DirectionalLightSource : public LightSource
{
public:
	DirectionalLightSource(void);
	~DirectionalLightSource(void);

	virtual FluxCollection * GetFluxes( const vector3 & point, const vector3 & normal ) const;

	const vector3 & direction() const { return m_direction; }
	void direction(const vector3 & val) { m_direction = val; }

	const color4 & color() const { return m_color; }
	void color(const color4 & val) { m_color = val; }


private:
	vector3 m_direction;
	color4 m_color;
};

