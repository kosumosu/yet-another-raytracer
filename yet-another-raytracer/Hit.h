#pragma once

#include "Ray.h"
#include "Types.h"

class GeometryObject;

class Hit
{
public:
	

	// Initializes successful hit
	Hit(const vector3 & point, const vector3 & normal, const GeometryObject * object, space_real distance)
		: m_object(object)
		, m_has_occurred(true)
		, m_point(point)
		, m_normal(normal)
		, m_distance(distance)
	{ }

	// Initializes unsuccessful hit
	Hit()
		: m_has_occurred(false)
	{

	}

	const GeometryObject * object() const { return m_object; }

	// Gets value determining that hit has occurred
	bool has_occurred() const { return m_has_occurred; }

	// Gets point where object was hit
	const vector3 & point() const { return m_point; }

	// Gets surface normal at point where object was hit
	const vector3 & normal() const { return m_normal; }

	// Gets distance from ray origin to hit point
	space_real distance() const { return m_distance; }

private:
	const GeometryObject * m_object;
	bool m_has_occurred;
	vector3 m_point;
	vector3 m_normal;
	space_real m_distance;
	
	// These are not needed now
	//vector3 m_tangent;
	//vector3 m_color;
	//vector3 m_texture_coords;

};
