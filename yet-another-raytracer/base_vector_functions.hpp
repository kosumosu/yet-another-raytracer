#pragma once

// fucking windows.h
#undef max
#undef min

#include "base_vector.hpp"
#include <algorithm>


namespace math
{
	template <typename T, size_t N>
	T inline dot( const base_vector<T, N> & one, const base_vector<T, N> & another)
	{
		static_assert(N > 0, "Vector must have at least one component.");

		T res = one[0] * another[0];
		for (size_t i = 1; i < N; i++)
		{
			res += one[i] * another[i];
		}
		return res;
	}

	template <typename T, size_t N>
	base_vector<T, N> inline dot_vec( const base_vector<T, N> & one, const base_vector<T, N> & another)
	{
		static_assert(N > 0, "Vector must have at least one component.");

		T res = one[0] * another[0];
		for (size_t i = 1; i < N; i++)
		{
			res += one[i] * another[i];
		}
		return base_vector<T, N>(res);
	}

	template <typename T, size_t N>
	T inline length2( const base_vector<T, N> & one )
	{
		return dot(one, one);
	}

	template <typename T, size_t N>
	T inline length( const base_vector<T, N> & one )
	{
		return std::sqrt(length2(one));
	}

	// Not guaranteed to be precise but may be faster.
	template <typename T, size_t N>
	T inline fast_length( const base_vector<T, N> & one )
	{
		return length(one);
	}

	template <typename T, size_t N>
	T inline inv_length( const base_vector<T, N> & one )
	{
		return T(1) / length(one);
	}

	// Not guaranteed to be precise but may be faster.
	template <typename T, size_t N>
	T inline fast_inv_length( const base_vector<T, N> & one )
	{
		return T(1) / fast_length(one);
	}

	template <typename T, size_t N>
	base_vector<T, N> inline normalize( const base_vector<T, N> & one )
	{
		return one * inv_length(one);
	}

	template <typename T, size_t N>
	base_vector<T, N> inline fast_normalize( const base_vector<T, N> & one )
	{
		return one * fast_inv_length(one);
	}

	template <typename T, size_t N>
	base_vector<T, N> inline reciprocal( const base_vector<T, N> & one )
	{
		return T(1) / one;
	}

	// Not guaranteed to be precise but may be faster.
	template <typename T, size_t N>
	base_vector<T, N> inline fast_reciprocal( const base_vector<T, N> & one )
	{
		return reciprocal(one);
	}

	template <typename T, size_t N>
	base_vector<T, N> inline max( const base_vector<T, N> & one, const base_vector<T, N> & another)
	{
		base_vector<T, N> res;
		for (size_t i = 0; i < N; i++)
		{
			res[i] = std::max(one[i], another[i]);
		}
		return res;
	}

	template <typename T, size_t N>
	base_vector<T, N> inline min( const base_vector<T, N> & one, const base_vector<T, N> & another)
	{
		base_vector<T, N> res;
		for (size_t i = 0; i < N; i++)
		{
			res[i] = std::min(one[i], another[i]);
		}
		return res;
	}

	template <typename T, size_t N>
	base_vector<T, N> inline clamp( const base_vector<T, N> & vec, const base_vector<T, N> & low, const base_vector<T, N> & high)
	{
		return min(high, max(low, vec));
	}

	template <typename T, size_t N>
	base_vector<T, N> inline abs( const base_vector<T, N> & one)
	{
		base_vector<T, N> res;
		for (size_t i = 0; i < N; i++)
		{
			res[i] = std::abs(one[i]);
		}
		return res;
	}

	template <typename T, size_t N>
	base_vector<T, N - 1> inline subvector( const base_vector<T, N> & vec, size_t element)
	{
		base_vector<T, N - 1> res;
		for (size_t i = 0; i < element; i++)
		{
			res[i] = vec[i];
		}

		for (size_t i = element; i < N - 1; i++)
		{
			res[i] = vec[i + 1];
		}

		return res;
	}

	template <typename T, size_t N, size_t ELEMENT>
	base_vector<T, N - 1> inline subvector( const base_vector<T, N> & vec)
	{
		static_assert(ELEMENT >= 0, "ELEMENT can't be negative");
		static_assert(ELEMENT < N, "ELEMENT must be less than N");

		base_vector<T, N - 1> res;
		for (size_t i = 0; i < ELEMENT; i++)
		{
			res[i] = vec[i];
		}

		for (size_t i = ELEMENT; i < N - 1; i++)
		{
			res[i] = vec[i + 1];
		}

		return res;
	}

	template <typename T, size_t N>
	base_vector<T, N> inline blend( const base_vector<T, N> & one, const base_vector<T, N> & another, const T & fraction)
	{
		T inv_fraction = T(1) - fraction;
		base_vector<T, N> res;
		for (size_t i = 1; i < N; i++)
		{
			res += one[i] * inv_fraction + another[i] * fraction;
		}
		return res;
	}

	template <typename T, size_t N>
	base_vector<T, N> inline blend( const base_vector<T, N> & one, const base_vector<T, N> & another, const base_vector<T, N> & fractions)
	{
		auto inv_fraction = base_vector<T, N>(T(1)) - fractions;
		base_vector<T, N> res = one * inv_fraction + another * fractions;
		return res;
	}
}