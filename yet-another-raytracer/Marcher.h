#pragma once

#include "objects/GeometryObject.h"

class Marcher
{
public:
	virtual ~Marcher() = default;


	virtual void Restart(const math::ray<space_real, 3> & ray, space_real near, space_real far) = 0;

	// Gets objects in current leaf.
	virtual const std::vector<objects::GeometryObject*> * GetCurrentObjects() = 0;

	// Marches to the next leaf.
	virtual bool MarcheNext() = 0;

	[[nodiscard]] virtual bool IsCorrectIntersectionForCurrentState(space_real dist) const = 0;
};

