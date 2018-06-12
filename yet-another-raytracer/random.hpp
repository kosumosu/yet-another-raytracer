#pragma once

#include "constants.hpp"
#include "vector.hpp"
#include <algorithm>
#include <random>

namespace math
{
	
	template <typename TValue, std::size_t N, typename TRandomEngine>
	vector<TValue, N> inline linearRand( const vector<TValue, N> & low, const vector<TValue, N> & high, TRandomEngine & engine)
	{
		std::uniform_real_distribution<TValue> distr;

		vector<TValue, N> rand = vector<TValue, N>::zero();
		iterate<0, N - 1>([&](std::size_t i)
		{
			rand[i] = distr(engine);
		});

		auto res = blend(low, high, rand);

		return res;
	}

	template <typename TValue, typename TRandomEngine>
	TValue inline linearRand( const TValue & low, const TValue & high, TRandomEngine & engine)
	{
		std::uniform_real_distribution<TValue> distr;

		return low + (high - low) * distr(engine);
	}

	template <typename TValue, typename TRandomEngine>
	vector<TValue, 3> inline sphericalRand(TRandomEngine & engine)
	{
		TValue z = linearRand(TValue(-1), TValue(1), engine);
		TValue a = linearRand(TValue(0), TValue(2.0 * pi), engine);

		TValue r = std::sqrt(TValue(1) - z * z);

		TValue x = r * std::cos(a);
		TValue y = r * std::sin(a);

		return vector<TValue, 3>(x, y, z);	
	}

	template <typename TValue, typename TRandomEngine>
	inline vector<TValue, 3> hemiSphericalRand(const vector<TValue, 3> & normal, TRandomEngine & engine)
	{
		auto initial_vector = sphericalRand<TValue>(engine);

		if (dot(normal, initial_vector) >= TValue(0))
			return initial_vector;
		else
			return -initial_vector;
	}

	template <typename TValue, typename TRandomEngine>
	inline vector<TValue, 2> circularRand(TRandomEngine & engine)
	{
		std::uniform_real_distribution<TValue> distr;

		TValue radius = std::sqrt(distr(engine));
		TValue theta = distr(engine) * TValue(2.0 * math::pi);

		return vector<TValue, 2>(radius * std::cos(theta), radius * std::sin(theta));
	}

	template <typename TValue, typename TRandomEngine>
	inline vector<TValue, 3> cosineWeightedHemiSphericalRand(TRandomEngine & engine)
	{
		auto pointOnCircle = circularRand(engine);

		return vector<TValue, 3>(pointOnCircle[0], pointOnCircle[1], std::sqrtf(std::max(0.f, 1.f - pointOnCircle[0] * pointOnCircle[0] - pointOnCircle[1] * pointOnCircle[1])));
	}

	template <typename TValue, typename TRandomEngine>
	inline vector<TValue, 3> cosineWeightedHemiSphericalRand(const vector<TValue, 3> & normal, TRandomEngine & engine)
	{
		// get some arbitrary vector that is not colinear with normal
		auto someVector = std::abs(normal[0]) < TValue(oneOverSqrt2)
			? vector<TValue, 3>(1, 0, 0)
			: vector<TValue, 3>(0, 1, 0);


		auto firstBinormal = math::normalize(math::cross(normal, someVector));
		auto secondBinormal = math::cross(normal, firstBinormal);

		std::uniform_real_distribution<TValue> distr;

		TValue randomNumber0 = distr(engine);
		TValue radius = std::sqrt(randomNumber0);
		TValue theta = distr(engine) * TValue(2.0 * math::pi);

		auto pointOnCircle = vector<TValue, 2>(radius * std::cos(theta), radius * std::sin(theta));

		return firstBinormal * pointOnCircle[0] + secondBinormal * pointOnCircle[1] + normal * (std::sqrt(TValue(1.0) - randomNumber0));
	}
}