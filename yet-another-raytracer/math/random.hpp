#pragma once

#include "constants.hpp"
#include "vector.hpp"
#include <algorithm>
#include <cassert>
#include <random>

#include "vector3_functions.hpp"

namespace math
{
	template <typename TValue, std::size_t N, typename TSampler>
	vector<TValue, N> linearRand(const vector<TValue, N>& low, const vector<TValue, N>& high, TSampler& sampler)

	
	{
		vector<TValue, N> rand = vector<TValue, N>::zero();
		iterate<0, N - 1>(
			[&](std::size_t i)
			{
				rand[i] = sampler.Get1D(); // Better have separate 2D implementation
			});

		auto res = blend(low, high, rand);

		return res;
	}

	template <typename TValue, typename TSampler>
	TValue linearRand(const TValue& low, const TValue& high, TSampler& sampler)
	{
		return low + (high - low) * sampler.Get1D();
	}

	template <typename TValue>
	TValue sampleExponential(const TValue& random_value, const TValue& a)
	{
		return -std::log(TValue(1.0) - random_value) / a;
	}

	template <typename TValue>
	TValue exponentialPDF(const TValue& value, const TValue& a)
	{
		return a * std::exp(-a * value);
	}

	template <typename TValue, typename TSampler>
	vector<TValue, 3> conicalRand(TSampler& sampler, const TValue& half_angular_size_cos)
	{
		const auto u = sampler.Get2D();

		const auto cosTheta = (1 - u[0]) + u[0] * half_angular_size_cos;
		const auto sinTheta = math::safeSqrt(1 - math::square(cosTheta));
		const auto phi = u[1] * 2 * std::numbers::pi_v<TValue>;
		return math::from_theta_phi_towards_x(sinTheta, cosTheta, phi);
	}

	template <typename TValue>
	TValue conicalRandPdf(const TValue& half_angular_size_cos)
	{
		auto val = 1 / (2 * std::numbers::pi_v<TValue> * (1 - half_angular_size_cos));
		return val;
	}

	template <typename TValue, typename TSampler>
	vector<TValue, 3> sphericalRand(TSampler& sampler)
	{
		TValue z = linearRand(TValue(-1), TValue(1), sampler);
		TValue a = linearRand(TValue(0), TValue(2.0 * pi), sampler);

		TValue r = std::sqrt(TValue(1) - z * z);

		TValue x = r * std::cos(a);
		TValue y = r * std::sin(a);

		return vector<TValue, 3>(x, y, z);
	}

	template <typename TValue, typename TSampler>
	vector<TValue, 3> hemiSphericalRand(const vector<TValue, 3>& normal, TSampler& sampler)
	{
		if (const auto initialVector = sphericalRand<TValue>(sampler); dot(normal, initialVector) >= TValue(0))
			return initialVector;
		else
			return -initialVector;
	}

	template <typename TValue, typename TSampler>
	vector<TValue, 2> circularRand(TSampler& sampler)
	{
		TValue radius = std::sqrt(sampler.Get1D());
		TValue theta = sampler.Get1D() * TValue(2.0 * math::pi);

		return vector<TValue, 2>(radius * std::cos(theta), radius * std::sin(theta));
	}

	template <typename TValue, typename TSampler>
	vector<TValue, 3> cosineWeightedHemiSphericalRand(TSampler& sampler)
	{
		auto pointOnCircle = circularRand(sampler);

		return vector<TValue, 3>(
			pointOnCircle[0],
			pointOnCircle[1],
			std::sqrt(std::max(0.f, 1.f - pointOnCircle[0] * pointOnCircle[0] - pointOnCircle[1] * pointOnCircle[1])));
	}

	template <typename TValue, typename TSampler>
	std::tuple<vector<TValue, 3>, TValue> cosineWeightedHemiSphericalRand(const vector<TValue, 3>& normal, TSampler& sampler)
	{
		// get some arbitrary vector that is not collinear with normal
		const auto someVector = std::abs(normal[0]) < TValue(oneOverSqrt2) ? vector<TValue, 3>(1, 0, 0) : vector<TValue, 3>(0, 1, 0);

		const auto firstBinormal = math::normalize(math::cross(normal, someVector));
		const auto secondBinormal = math::cross(normal, firstBinormal);

		const TValue randomNumber0 = sampler.Get1D();
		const TValue randomNumber1 = sampler.Get1D();

		assert(randomNumber0 < 1.0 - 1e-40);

		const TValue normalCosine = std::sqrt(TValue(1.0) - randomNumber0);
		const TValue radius = std::sqrt(randomNumber0);
		const TValue theta = randomNumber1 * TValue(2.0 * math::pi);

		const auto pointOnCircle = vector<TValue, 2>(radius * std::cos(theta), radius * std::sin(theta));

		const auto point = firstBinormal * pointOnCircle[0] + secondBinormal * pointOnCircle[1] + normal * normalCosine;
		return {point, normalCosine * TValue(oneOverPi)};
	}


	template <class TValue, std::size_t N>
	constexpr std::size_t sample_vector_component(const vector<TValue, N>& normalized_weights, const TValue& random_value)
	{
		TValue weight_sum = 0;
		for (std::size_t i = 0; i < N - 1; ++i)
		{
			weight_sum += normalized_weights[i];
			if (random_value < weight_sum)
				return i;
		}
		if (!(weight_sum + normalized_weights[N - 1] > 0.999 && weight_sum + normalized_weights[N - 1] < 1.001))
		{
			int s = 03;
		}
		assert(weight_sum + normalized_weights[N - 1] > 0.999 && weight_sum + normalized_weights[N - 1] < 1.001);
		return N - 1;
	}
}
