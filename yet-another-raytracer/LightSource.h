#pragma once

#include "UniformRandomBitGenerator.h"
#include "Types.h"
#include <vector>
#include <memory>
#include <functional>
#include "Distribution.h"

class LightingContext;
class RayEvaluator;

struct light_sample
{
	light_sample(const vector3 & direction, const space_real & distance, const std::function<color_rgbx()> & evaluate)
		: direction(direction),
		  distance(distance),
		  evaluate(evaluate) { }

	light_sample(vector3 && direction, space_real && distance, std::function<color_rgbx()> && evaluate)
		: direction(std::move(direction)),
		  distance(std::move(distance)),
		  evaluate(std::move(evaluate)) { }

	vector3 direction;
	space_real distance;
	const std::function<color_rgbx()> evaluate;
};

using light_distribution = Distribution<const light_sample, const vector3, space_real>;
using distibution_func = std::function<void(const light_distribution & job)>;

class LightSource
{
public:

	LightSource(void) { }

	virtual ~LightSource(void) { }

	virtual void DoWithDistribution(const LightingContext & context, math::UniformRandomBitGenerator<unsigned int> & randomEngine, const distibution_func & job) const = 0;
};

typedef std::vector<std::shared_ptr<LightSource>> LightSourceCollection;
