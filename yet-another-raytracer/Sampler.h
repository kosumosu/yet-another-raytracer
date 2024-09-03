#pragma once

#include "math/vector.hpp"

#include <memory>
#include <random>

namespace math
{
	template <typename T>
	constexpr T upperRandomBound; // = std::nextafter(T(1.0), T(0.0)); // this requires constexpr cmath.

	template <>
	constexpr float upperRandomBound<float> = 0.99999994039535522461; // one step away from 1.0f

	template <>
	constexpr double upperRandomBound<double> = 0.99999999999999988898; // one step away from 1.0

	template <typename T>
	constexpr T lowerRandomBound;

	template <>
	constexpr float lowerRandomBound<float> = 5.605193857e-45f; // five steps away from 0.0f to avoid numeric problems.

	template <>
	constexpr double lowerRandomBound<double> = 1.97626258336498618e-323; // five steps away from 0.0 to avoid numeric problems.

	
	template <typename TValue>
	class Sampler
	{
	public:
		using result_type = TValue;

		virtual ~Sampler() = default;

		// Samples a uniformly distributed random number in the range [0; 1)
		[[nodiscard]] virtual TValue Get1D() = 0;

		[[nodiscard]] virtual math::vector<TValue, 2> Get2D() = 0;
	};

	template <typename TValue, typename TEngine/*, class = typename std::enable_if_t<std::is_same<TValue, typename TEngine::result_type>::value>*/>
	class SimpleSampler : public Sampler<TValue>
	{
		mutable TEngine engine_;
		mutable std::uniform_real_distribution<TValue> distr_;
	public:
		explicit SimpleSampler(TEngine engine)
			: engine_{ std::move(engine) }
			, distr_{ TValue(0), std::min(TValue(1) - getGrain(engine_, TValue{}), upperRandomBound<TValue>) }
		{
		}

		TValue Get1D() override
		{
			auto number =  distr_(engine_);
			assert(number < 1.0 - 1e-40);
			return number;
		}

		math::vector<TValue, 2> Get2D() override
		{
			return { Get1D(), Get1D() };
		}

	private:
		static float getGrain(const TEngine& engine, float)
		{
			return float(1.0) / (float(engine.max()) - float(engine.min()));  // TODO: make sure it is really how distr_ works inside.
		}

		static double getGrain(const TEngine& engine, double)
		{
			const auto engineSpan = double(engine.max()) - double(engine.min());
			return double(1.0) / (engineSpan * engineSpan); // TODO: make sure it is really how distr_ works inside.
		}
	};
}