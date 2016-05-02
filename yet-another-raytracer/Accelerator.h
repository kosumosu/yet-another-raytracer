#pragma once

#include "Marcher.h"
#include "Ray.h"

class Accelerator
{
public:

	Accelerator(void)
	{
	}

	virtual ~Accelerator(void)
	{
	}

	//virtual Marcher * StartMarching( const Ray & ray, space_real near, space_real far ) const = 0;
	virtual Marcher * CreateMarcher() const = 0;
};

