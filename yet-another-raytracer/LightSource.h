#pragma once

#include "Sampler.h"
#include "Types.h"
#include "Distribution.h"
#include <vector>
#include <memory>
#include <functional>
#include <optional>

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

// TSample is optional because light can sometimes beforehand tell it will not return light from this sample.
using light_distribution = Distribution<std::optional<light_sample>, vector3, space_real>;
using distibution_func = std::function<void(const light_distribution& job)>;

class LightSource
{
public:

	LightSource() = default;

	virtual ~LightSource() = default;

	virtual void DoWithDistribution(const LightingContext& context, math::Sampler<space_real>& sampler,
	                                const distibution_func& job) const = 0;

	virtual color_real GetApproximateTotalPower() const = 0;
};

typedef std::vector<std::shared_ptr<LightSource>> LightSourceCollection;
