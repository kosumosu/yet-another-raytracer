#pragma once

#include "Types.h"
#include <vector>

class LightSource;

class Flux
{
public:
	//Flux()
	//{
	//}


	// direction is direction from point to light
	Flux(const LightSource * source, const vector3 & direction, const color_rgbx & color, space_real distance, color_real probabilityDensity)
		: m_source(source)
		, m_direction(direction)
		, m_color(color)
		, m_distance(distance)
		, m_probabilityDensity(probabilityDensity)
	{

	}

	const LightSource * source() const { return m_source; }
	//void source(const LightSource * val) { m_source = val; }

	// Direction to light source
	const vector3 & direction() const { return m_direction; }
	//void direction(const vector3 & val) { m_direction = val; }

	// Resulting color of illumination. Already attenuated
	const color_rgbx & color() const { return m_color; }
	//void color(const color_rgbx & val) { m_color = val; }

	// Gets distance to light source [atom]
	space_real distance() const { return m_distance; }
	//void distance(space_real val) { m_distance = val; }

	color_real probabilityDensity() const { return m_probabilityDensity; }

private:
	const LightSource * m_source;
	vector3 m_direction;
	color_rgbx m_color;
	space_real m_distance;
	color_real m_probabilityDensity;
};

typedef std::vector< Flux > FluxCollection;