#pragma once

#include "Types.h"
#include <vector>

class LightSource;

class Flux
{
public:
	// direction is direction from surface point to light
	Flux(const LightSource * source, const vector3 & direction, const color_rgbx & color, space_real distance, color_real probabilityDensity)
		: m_source(source)
		, m_direction(direction)
		, m_color(color)
		, m_distance(distance)
		, m_probabilityDensity(probabilityDensity)
	{

	}

	const LightSource * source() const { return m_source; }

	// Direction to light source
	const vector3 & direction() const { return m_direction; }

	// Resulting color of illumination. Already attenuated
	const color_rgbx & color() const { return m_color; }

	// Gets distance to light source
	space_real distance() const { return m_distance; }

	color_real probabilityDensity() const { return m_probabilityDensity; }

private:
	const LightSource * m_source; // is it needed here?
	vector3 m_direction;
	color_rgbx m_color;
	space_real m_distance;
	color_real m_probabilityDensity;
};