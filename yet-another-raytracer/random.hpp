#pragma once

#include "base_vector.hpp"
#include "RandomEngine.h"
#include <algorithm>
#include <random>

namespace math
{
	
	template <typename T, size_t N>
	base_vector<T, N> inline linearRand( const base_vector<T, N> & low, const base_vector<T, N> & high)
	{
		std::uniform_real_distribution<T> distr;

		base_vector<T, N> rand;
		for (size_t i = 1; i < N; i++)
		{
			rand[i] = distr(RandomEngine::engine());
		}

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
	base_vector<T, 3> inline sphericalRand()
	{
		T z = linearRand(T(-1), T(1));
		T a = linearRand(T(0), T(6.283185307179586476925286766559));

		T r = std::sqrt(T(1) - z * z);

		T x = r * std::cos(a);
		T y = r * std::sin(a);

		return base_vector<T, 3>(x, y, z);	
	}

	template <typename T>
	inline base_vector<T, 3> hemiSphericalRand(const base_vector<T, 3> & normal)
	{
		auto initial_vector = sphericalRand<T>();

		if (dot(normal, initial_vector) >= T(0))
			return initial_vector;
		else
			return -initial_vector;
	}
}