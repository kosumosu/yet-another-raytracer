#pragma once

#include "GeometryObject.h"

class Marcher
{
public:

	Marcher(void)
	{
	}

	virtual ~Marcher(void)
	{
	}

	// Gets objects in current leaf.
	virtual const ObjectCollection * GetCurrentObjects() = 0;

	// Marches to the next leaf.
	virtual bool MarcheNext() = 0;

};

