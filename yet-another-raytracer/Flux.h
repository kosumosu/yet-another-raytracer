#pragma once

#include "Types.h"
#include <vector>

class LightSource;

class __declspec(align(MM_ALIGNMENT)) Flux
{
public:
	Flux()
	{
	}


	// direction is direction from point to light
	Flux(const LightSource * source, const vector3 & direction, const color4 & color, space_real distance)
		: m_source(source)
		, m_direction(direction)
		, m_color(color)
		, m_distance(distance)
	{

	}

	const LightSource * source() const { return m_source; }
	void source(const LightSource * val) { m_source = val; }

	// Direction to light source
	const vector3 & direction() const { return m_direction; }
	void direction(const vector3 & val) { m_direction = val; }

	// Resulting color of illumination. Already attenuated
	const color4 & color() const { return m_color; }
	void color(const color4 & val) { m_color = val; }

	// Gets distance to light source [atom]
	space_real distance() const { return m_distance; }
	void distance(space_real val) { m_distance = val; }

private:
	const LightSource * m_source;
	vector3 m_direction;
	color4 m_color;
	space_real m_distance;
};

typedef std::vector< Flux > FluxCollection;