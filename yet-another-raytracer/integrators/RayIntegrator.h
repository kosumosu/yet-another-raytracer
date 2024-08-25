#pragma once

#include "Types.h"
#include "Sampler.h"

class RayIntegrator
{
public:
	virtual ~RayIntegrator() = default;
	virtual color_rgbx EvaluateRay(const ray3 & ray, unsigned int bounceLimit, space_real bias, math::Sampler<space_real>& sampler) = 0;
};
