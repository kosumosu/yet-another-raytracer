#pragma once

#include "Types.h"
#include "Sampler.h"

class RayIntegrator
{
public:
	virtual ~RayIntegrator() = default;
	virtual color_rgb EvaluateRay(const ray3 & ray, space_real bias, math::Sampler<space_real>& sampler) = 0;
};
