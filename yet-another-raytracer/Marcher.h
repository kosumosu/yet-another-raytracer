#pragma once

#include "GeometryObject.h"

class Marcher
{
protected:
	Marcher(void)
	{
	}

public:
	virtual ~Marcher(void)
	{
	}


	virtual void Restart(const Ray & ray, space_real near, space_real far) = 0;

	// Gets objects in current leaf.
	virtual const std::vector<GeometryObject*> * GetCurrentObjects() = 0;

	// Marches to the next leaf.
	virtual bool MarcheNext() = 0;

	virtual bool IsDistanceWithinCurrentBounds(space_real dist) const = 0;
};

