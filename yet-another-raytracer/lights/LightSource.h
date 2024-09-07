#pragma once

#include "Sampler.h"
#include "Types.h"
#include "Distribution.h"
#include <vector>
#include <memory>
#include <functional>
#include <optional>


class RayEvaluator;

namespace lights
{
	class LightingContext;

	struct light_sample
	{
		template <class TDirection, class TDistance, class TEvaluate>
		light_sample(TDirection direction, TDistance distance, TEvaluate evaluate)
			: direction(std::move(direction)),
			  distance(std::move(distance)),
			  evaluate(std::move(evaluate))
		{
		}

		vector3 direction;
		space_real distance;
		const std::function<color_rgb()> evaluate;
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
										const distibution_func& job) const noexcept = 0;

		virtual void DoWithDistribution(const vector3& point, math::Sampler<space_real>& sampler,
									const distibution_func& job) const noexcept = 0;

		// Not guaranteed to be correct yet.
		[[nodiscard]] virtual space_real EvaluatePdfExperimental(const ray3& ray) const noexcept = 0;

		[[nodiscard]] virtual color_real GetApproximateTotalPower() const noexcept = 0;
	};

	using LightSourceCollection = std::vector<std::shared_ptr<LightSource>>;
}