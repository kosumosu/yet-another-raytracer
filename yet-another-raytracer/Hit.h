#pragma once

#include <utility>

#include "ray.hpp"
#include "Types.h"

class GeometryObject;

class Hit
{
public:
	// Initializes successful hit
	Hit(vector3 point, vector3 normal, const GeometryObject* object, space_real distance, uvs_t uvs)
		: _object(object)
		, _has_occurred(true)
		, _point(std::move(point))
		, _normal(std::move(normal))
		, _distance(distance)
		, _uvs(std::move(uvs))
	{
	}

	// Initializes unsuccessful hit
	Hit()
		: _object(nullptr)
		, _has_occurred(false)
		, _point(vector3::zero())
		, _normal(vector3::zero())
		, _distance(0)
		, _uvs({vector2(0, 0)})
	{
	}

	const GeometryObject* object() const { return _object; }

	// Gets value determining that hit has occurred
	bool has_occurred() const { return _has_occurred; }

	// Gets point where object was hit
	const vector3& point() const { return _point; }

	// Gets surface normal at point where object was hit
	const vector3& normal() const { return _normal; }

	// Gets distance from ray origin to hit point
	space_real distance() const { return _distance; }

	const uvs_t& uvs() const { return _uvs; }
	uvs_t& uvs() { return _uvs; }

private:
	const GeometryObject* _object;
	bool _has_occurred;
	vector3 _point;
	vector3 _normal;
	space_real _distance;
	uvs_t _uvs;
};
