#pragma once

#include "Constants.h"
#include "vector.hpp"
#include "RandomEngine.h"
#include <algorithm>
#include <random>

namespace math
{
	
	template <typename T, size_t N>
	vector<T, N> inline linearRand( const vector<T, N> & low, const vector<T, N> & high)
	{
		std::uniform_real_distribution<T> distr;

		vector<T, N> rand;
		iterate<0, N - 1>([&](size_t i)
		{
			rand[i] = distr(RandomEngine::engine());
		});

		auto res = blend(low, high, rand);

		return res;
	}

	template <typename T>
	T inline linearRand( const T & low, const T & high)
	{
		std::uniform_real_distribution<T> distr;

		return low + (high - low) * distr(RandomEngine::engine());
	}

	template <typename T>
	vector<T, 3> inline sphericalRand()
	{
		T z = linearRand(T(-1), T(1));
		T a = linearRand(T(0), T(2.0 * pi));

		T r = std::sqrt(T(1) - z * z);

		T x = r * std::cos(a);
		T y = r * std::sin(a);

		return vector<T, 3>(x, y, z);	
	}

	template <typename T>
	inline vector<T, 3> hemiSphericalRand(const vector<T, 3> & normal)
	{
		auto initial_vector = sphericalRand<T>();

		if (dot(normal, initial_vector) >= T(0))
			return initial_vector;
		else
			return -initial_vector;
	}

	template <typename T>
	inline vector<T, 2> circularRand()
	{
		std::uniform_real_distribution<T> distr;

		T radius = std::sqrt(distr(RandomEngine::engine()));
		T theta = distr(RandomEngine::engine()) * T(2.0 * math::pi);

		return vector<T, 2>(radius * std::cos(theta), radius * std::sin(theta));
	}

	template <typename T>
	inline vector<T, 3> cosineWeightedHemiSphericalRand()
	{
		auto pointOnCircle = circularRand();

		return vector<T, 3>(pointOnCircle[0], pointOnCircle[1], std::sqrtf(std::max(0.f, 1.f - pointOnCircle[0] * pointOnCircle[0] - pointOnCircle[1] * pointOnCircle[1])));
	}

	template <typename T>
	inline vector<T, 3> cosineWeightedHemiSphericalRand(const vector<T, 3> & normal)
	{
		// get some arbitrary vector that is not colinear with normal
		auto someVector = std::abs(normal[0]) < T(oneOverSqrt2)
			? vector<T, 3>(1, 0, 0)
			: vector<T, 3>(0, 1, 0);


		auto firstBinormal = math::normalize(math::cross(normal, someVector));
		auto secondBinormal = math::cross(normal, firstBinormal);

		std::uniform_real_distribution<T> distr;

		T randomNumber0 = distr(RandomEngine::engine());
		T radius = std::sqrt(randomNumber0);
		T theta = distr(RandomEngine::engine()) * T(2.0 * math::pi);

		auto pointOnCircle = vector<T, 2>(radius * std::cos(theta), radius * std::sin(theta));

		return firstBinormal * pointOnCircle[0] + secondBinormal * pointOnCircle[1] + normal * (std::sqrt(T(1.0) - randomNumber0));
	}
}