#pragma once

#include "vector.hpp"
#include "helper.hpp"
#include <algorithm>

namespace math
{
	template <typename T, std::size_t N>
	T dot( const vector<T, N> & one, const vector<T, N> & another)
	{
		static_assert(N > 0, "Vector must have at least one component.");

		T res = one[0] * another[0];
		iterate<1, N - 1>([&](std::size_t i)
		{
			res += one[i] * another[i];
		});

		return res;
	}

	template <typename T, std::size_t N>
	bool is_acute_angle(const vector<T, N> & one, const vector<T, N> & another)
	{
		return dot(one, another) > 0;
	}

	template <typename T, std::size_t N>
	bool is_obtuse_angle(const vector<T, N> & one, const vector<T, N> & another)
	{
		return dot(one, another) < 0;
	}

	// Computes dot product and populates vector with this value.
	template <typename T, std::size_t N>
	vector<T, N> dot_vec( const vector<T, N> & one, const vector<T, N> & another)
	{
		static_assert(N > 0, "Vector must have at least one component.");

		return vector<T, N>(dot(one, another));
	}

	template <typename T, std::size_t N>
	T length2( const vector<T, N> & one )
	{
		return dot(one, one);
	}

	template <typename T, std::size_t N>
	T length( const vector<T, N> & one )
	{
		return std::sqrt(length2(one));
	}

	// Not guaranteed to be precise but may be faster.
	template <typename T, std::size_t N>
	T fast_length( const vector<T, N> & one )
	{
		return length(one);
	}

	template <typename T, std::size_t N>
	T inv_length( const vector<T, N> & one )
	{
		return T(1) / length(one);
	}

	// Not guaranteed to be precise but may be faster.
	template <typename T, std::size_t N>
	T fast_inv_length( const vector<T, N> & one )
	{
		return T(1) / fast_length(one);
	}

	template <typename T, std::size_t N>
	vector<T, N> normalize( const vector<T, N> & one )
	{
		return one * inv_length(one);
	}

	template <typename T, std::size_t N>
	vector<T, N> fast_normalize( const vector<T, N> & one )
	{
		return one * fast_inv_length(one);
	}

	template <typename T, std::size_t N>
	vector<T, N> reciprocal( const vector<T, N> & one )
	{
		return T(1) / one;
	}

	// Not guaranteed to be precise but may be faster.
	template <typename T, std::size_t N>
	vector<T, N> fast_reciprocal( const vector<T, N> & one )
	{
		return reciprocal(one);
	}

	template <typename T, std::size_t N>
	vector<T, N> max( const vector<T, N> & one, const vector<T, N> & another)
	{
		vector<T, N> res = vector<T, N>::zero();
		iterate<0, N - 1>([&](std::size_t i)
		{
			res[i] = std::max(one[i], another[i]);
		});
		return res;
	}

	template <typename T, std::size_t N>
	vector<T, N> min( const vector<T, N> & one, const vector<T, N> & another)
	{
		vector<T, N> res = vector<T, N>::zero();
		iterate<0, N - 1>([&](std::size_t i)
		{
			res[i] = std::min(one[i], another[i]);
		});
		return res;
	}

	template <typename T, std::size_t N>
	T max_element(const vector<T, N> & one)
	{
		T res = one[0];
		iterate<1, N - 1>([&](std::size_t i)
		{
			res = std::max(res, one[i]);
		});
		return res;
	}

	template <typename T, std::size_t N>
	T min_element(const vector<T, N> & one)
	{
		T res = one[0];
		iterate<1, N - 1>([&](std::size_t i)
		{
			res = std::min(res, one[i]);
		});
		return res;
	}

	template <typename T, std::size_t N>
	vector<T, N> clamp(const vector<T, N> & vec, const vector<T, N> & low, const vector<T, N> & high)
	{
		return min(high, max(low, vec));
	}

	template <typename T, std::size_t N>
	vector<T, N> abs(const vector<T, N> & one)
	{
		vector<T, N> res = vector<T, N>::zero();
		iterate<0, N - 1>([&](std::size_t i)
		{
			res[i] = std::abs(one[i]);
		});
		return res;
	}

	template <typename T, std::size_t N>
	vector<T, N - 1> subvector( const vector<T, N> & vec, std::size_t element)
	{
		vector<T, N - 1> res = vector<T, N - 1>::zero();
		for (std::size_t i = 0; i < element; i++)
		{
			res[i] = vec[i];
		}

		for (std::size_t i = element; i < N - 1; i++)
		{
			res[i] = vec[i + 1];
		}

		return res;
	}

	template <typename T, std::size_t N, std::size_t ELEMENT_TO_EXCLUDE, class = std::enable_if_t<(ELEMENT_TO_EXCLUDE >= 0)>>
	vector<T, N - 1> subvector( const vector<T, N> & vec)
	{
		static_assert(N > 1, "N must be greater then 1!");
		static_assert(ELEMENT_TO_EXCLUDE >= 0, "ELEMENT_TO_EXCLUDE can't be negative!");
		static_assert(ELEMENT_TO_EXCLUDE < N, "ELEMENT_TO_EXCLUDE must be less than N!");

		vector<T, N - 1> res = vector<T, N - 1>::zero();
		iterate<0, int(ELEMENT_TO_EXCLUDE) - 1>([&](std::size_t i)
		{
			res[i] = vec[i];
		});

		iterate<int(ELEMENT_TO_EXCLUDE), int(N) - 2>([&](std::size_t i)
		{
			res[i] = vec[i + 1];
		});

		return res;
	}

	template <typename T, std::size_t N>
	vector<T, N> blend( const vector<T, N> & one, const vector<T, N> & another, const T & fraction)
	{
		const T inv_fraction = T(1) - fraction;
		vector<T, N> res = vector<T, N>::zero();
		iterate<0, N - 1>([&](std::size_t i)
		{
			res[i] = one[i] * inv_fraction + another[i] * fraction;
		});
		return res;
	}

	template <typename T, std::size_t N>
	vector<T, N> blend( const vector<T, N> & one, const vector<T, N> & another, const vector<T, N> & fractions)
	{
		const auto inv_fraction = vector<T, N>::fill(1) - fractions;
		vector<T, N> res = one * inv_fraction + another * fractions;
		return res;
	}

	template <typename T, std::size_t N>
	vector<T, N> pow(const vector<T, N> & one, const T & power)
	{
		vector<T, N> res = vector<T, N>::zero();
		iterate<0, N - 1>([&](std::size_t i)
		{
			res[i] = std::pow(one[i], power);
		});
		return res;
	}

	template <typename T, std::size_t N>
	vector<T, N> pow(const vector<T, N> & one, const vector<T, N> & another)
	{
		vector<T, N> res = vector<T, N>::zero();
		iterate<0, N - 1>([&](std::size_t i)
		{
			res[i] = std::pow(one[i], another[i]);
		});
		return res;
	}

	template <typename T, std::size_t N>
	vector<T, N> intDivideRoundUp(const vector<T, N> & one, const vector<T, N> & another)
	{
		vector<T, N> res = vector<T, N>::zero();
		iterate<0, N - 1>([&](std::size_t i)
		{
			res[i] = (one[i] + another[i] - 1) / another[i];
		});
		return res;
	}
}