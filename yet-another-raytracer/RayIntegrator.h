#pragma once

#include "Types.h"
#include "UniformRandomBitGenerator.h"

class RayIntegrator
{
public:
	virtual ~RayIntegrator() = default;
	virtual color_rgbx EvaluateRay(const ray3 & ray, unsigned int bounceLimit, space_real bias, math::UniformRandomBitGenerator<random_int_t>& randomEngine) const = 0;
};
