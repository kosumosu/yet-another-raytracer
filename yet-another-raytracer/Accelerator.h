#pragma once

#include "Marcher.h"
#include "ray.hpp"

#include <memory>

class Accelerator
{
public:

	Accelerator(void)
	{
	}

	virtual ~Accelerator(void)
	{
	}

	//virtual Marcher * StartMarching( const ray & ray, space_real near, space_real far ) const = 0;
	virtual std::unique_ptr<Marcher> CreateMarcher() const = 0;
};

