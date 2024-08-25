#pragma once

#include "objects/GeometryObject.h"
#include "Types.h"

class LightingContext
{
public:
	LightingContext(const objects::GeometryObject* object, const vector3& point, const vector3& normal, space_real bias, unsigned depthLeft, bool allowSubdivision)
		: object_(object)
		, point_(point)
		, normal_(normal)
		, bias_(bias)
		, depthLeft_(depthLeft)
		, allowSubdivision_(allowSubdivision)
	{
	}

	const objects::GeometryObject* object() const { return object_; }

	vector3 getPoint() const
	{
		return point_;
	}

	vector3 getNormal() const
	{
		return normal_;
	}

	space_real getBias() const
	{
		return bias_;
	}

	unsigned getDepthLeft() const
	{
		return depthLeft_;
	}

	bool getAllowSubdivision() const
	{
		return allowSubdivision_;
	}

private:
	const objects::GeometryObject* object_;
	const vector3& point_;
	const vector3& normal_;
	const space_real bias_;
	const unsigned int depthLeft_;
	const bool allowSubdivision_;
};
