#pragma once

#include "constants.hpp"
#include "vector.hpp"
#include <algorithm>
#include <random>

namespace math
{
	// to workaround upper bound issue with STL PRNG
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

	// std::uniform_real_distribution frequently generates very small numbers (equal to lower bound) with too high probability when using fast floating point math. Locally forcing precise math for generation.
#pragma float_control( precise, off, push)
	
	template <typename TValue, std::size_t N, typename TRandomEngine>
	vector<TValue, N> linearRand(const vector<TValue, N>& low, const vector<TValue, N>& high, TRandomEngine& engine)
	{
		std::uniform_real_distribution<TValue> distr;

		vector<TValue, N> rand = vector<TValue, N>::zero();
		iterate<0, N - 1>(
			[&](std::size_t i)
			{
				rand[i] = distr(engine);
			});

		auto res = blend(low, high, rand);

		return res;
	}

	template <typename TValue, typename TRandomEngine>
	TValue linearRand(const TValue& low, const TValue& high, TRandomEngine& engine)
	{
		std::uniform_real_distribution<TValue> distr;

		return low + (high - low) * distr(engine);
	}

	template <typename TValue, typename TRandomEngine>
	vector<TValue, 3> sphericalRand(TRandomEngine& engine)
	{
		TValue z = linearRand(TValue(-1), TValue(1), engine);
		TValue a = linearRand(TValue(0), TValue(2.0 * pi), engine);

		TValue r = std::sqrt(TValue(1) - z * z);

		TValue x = r * std::cos(a);
		TValue y = r * std::sin(a);

		return vector<TValue, 3>(x, y, z);
	}

	template <typename TValue, typename TRandomEngine>
	vector<TValue, 3> hemiSphericalRand(const vector<TValue, 3>& normal, TRandomEngine& engine)
	{
		if (const auto initialVector = sphericalRand<TValue>(engine); dot(normal, initialVector) >= TValue(0))
			return initialVector;
		else
			return -initialVector;
	}

	template <typename TValue, typename TRandomEngine>
	vector<TValue, 2> circularRand(TRandomEngine& engine)
	{
		std::uniform_real_distribution<TValue> distr;

		TValue radius = std::sqrt(distr(engine));
		TValue theta = distr(engine) * TValue(2.0 * math::pi);

		return vector<TValue, 2>(radius * std::cos(theta), radius * std::sin(theta));
	}

	template <typename TValue, typename TRandomEngine>
	vector<TValue, 3> cosineWeightedHemiSphericalRand(TRandomEngine& engine)
	{
		auto pointOnCircle = circularRand(engine);

		return vector<TValue, 3>(
			pointOnCircle[0],
			pointOnCircle[1],
			std::sqrtf(std::max(0.f, 1.f - pointOnCircle[0] * pointOnCircle[0] - pointOnCircle[1] * pointOnCircle[1])));
	}


	// Returns random representing a point on the unit hemisphere. Along with the corresponding PDF value.
	template <typename TValue, typename TRandomEngine>
	std::tuple<vector<TValue, 3>, TValue> cosineWeightedHemiSphericalRand(const vector<TValue, 3>& normal, TRandomEngine& engine)
	{
		// get some arbitrary vector that is not collinear with normal
		const auto someVector = std::abs(normal[0]) < TValue(oneOverSqrt2) ? vector<TValue, 3>(1, 0, 0) : vector<TValue, 3>(0, 1, 0);

		const auto firstBinormal = math::normalize(math::cross(normal, someVector));
		const auto secondBinormal = math::cross(normal, firstBinormal);

		std::uniform_real_distribution<TValue> distr { lowerRandomBound<TValue>, TValue(1.0) };

		const TValue randomNumber0 = distr(engine);

		const TValue normalCosine = std::sqrt(randomNumber0);
		const TValue radius = std::sqrt(TValue(1.0) - randomNumber0);
		const TValue theta = distr(engine) * TValue(2.0 * math::pi);

		const auto pointOnCircle = vector<TValue, 2>(radius * std::cos(theta), radius * std::sin(theta));

		const auto point = firstBinormal * pointOnCircle[0] + secondBinormal * pointOnCircle[1] + normal * normalCosine;
		return { point, normalCosine * TValue(oneOverPi) };
}
#pragma float_control( pop )
}
