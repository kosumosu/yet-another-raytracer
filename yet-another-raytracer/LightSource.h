#pragma once

#include "UniformRandomBitGenerator.h"
#include "Types.h"
#include "Distribution.h"
#include <vector>
#include <memory>
#include <functional>

class LightingContext;
class RayEvaluator;

struct light_sample
{
	template <class TDirection, class TDistance, class TEvaluate>
	light_sample(TDirection&& direction, TDistance&& distance, TEvaluate&& evaluate)
		: direction(std::forward<TDirection>(direction)),
		  distance(std::forward<TDistance>(distance)),
		  evaluate(std::forward<TEvaluate>(evaluate))
	{
	}

	vector3 direction;
	space_real distance;
	const std::function<color_rgbx()> evaluate;
};

using light_distribution = Distribution<const light_sample, const vector3, space_real>;
using distibution_func = std::function<void(const light_distribution& job)>;

class LightSource
{
public:

	LightSource() = default;

	virtual ~LightSource() = default;

	virtual void DoWithDistribution(const LightingContext& context, math::UniformRandomBitGenerator<unsigned int>& randomEngine,
	                                const distibution_func& job) const = 0;

	virtual color_real GetApproximateTotalPower() const = 0;
};

typedef std::vector<std::shared_ptr<LightSource>> LightSourceCollection;
