#pragma once

#include "Types.h"

class LightingContext
{
public:
	LightingContext(const vector3 & point, const vector3 & normal, space_real bias, unsigned depthLeft, bool allowSubdivision)
		: _point(point),
		  _normal(normal),
		  _bias(bias),
		  _depthLeft(depthLeft),
		  _allowSubdivision(allowSubdivision) {}


	vector3 getPoint() const
	{
		return _point;
	}

	vector3 getNormal() const
	{
		return _normal;
	}

	space_real getBias() const
	{
		return _bias;
	}

	unsigned getDepthLeft() const
	{
		return _depthLeft;
	}

	bool getAllowSubdivision() const
	{
		return _allowSubdivision;
	}

private:
	const vector3 & _point;
	const vector3 & _normal;
	space_real _bias;
	unsigned int _depthLeft;
	bool _allowSubdivision;
};