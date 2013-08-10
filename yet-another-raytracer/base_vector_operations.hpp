#pragma once

// fucking windows.h
#undef max
#undef min

#include "base_vector.hpp"
#include <algorithm>


namespace math
{
	template <typename T, size_t N>
	bool inline operator ==( const base_vector<T, N> & one, const base_vector<T, N> & another)
	{
		for (size_t i = 0; i < N; i++)
		{
			if (one[i] != another[i])
				return false;
		}
		return true;
	}

	template <typename T, size_t N>
	bool inline operator !=( const base_vector<T, N> & one, const base_vector<T, N> & another)
	{
		return !(one == another);
	}

	template <typename T, size_t N>
	base_vector<T, N> inline operator +( const base_vector<T, N> & one, const base_vector<T, N> & another)
	{
		base_vector<T, N> res;
		for (size_t i = 0; i < N; i++)
		{
			res[i] = one[i] + another[i];
		}
		return res;
	}

	template <typename T, size_t N>
	base_vector<T, N> inline operator +=( base_vector<T, N> & one, const base_vector<T, N> & another)
	{
		for (size_t i = 0; i < N; i++)
		{
			one[i] += another[i];
		}
		return one;
	}

	template <typename T, size_t N>
	base_vector<T, N> inline operator -( const base_vector<T, N> & one, const base_vector<T, N> & another)
	{
		base_vector<T, N> res;
		for (size_t i = 0; i < N; i++)
		{
			res[i] = one[i] - another[i];
		}
		return res;
	}

	template <typename T, size_t N>
	base_vector<T, N> inline operator -=( base_vector<T, N> & one, const base_vector<T, N> & another)
	{
		for (size_t i = 0; i < N; i++)
		{
			one[i] -= another[i];
		}
		return one;
	}

	template <typename T, size_t N>
	base_vector<T, N> inline operator -( const base_vector<T, N> & one)
	{
		base_vector<T, N> res;
		for (size_t i = 0; i < N; i++)
		{
			res[i] = -one[i];
		}
		return res;
	}

	template <typename T, size_t N>
	base_vector<T, N> inline operator *( const base_vector<T, N> & one, const base_vector<T, N> & another)
	{
		base_vector<T, N> res;
		for (size_t i = 0; i < N; i++)
		{
			res[i] = one[i] * another[i];
		}
		return res;
	}

	template <typename T, size_t N>
	base_vector<T, N> inline operator *( const base_vector<T, N> & one, const T & scalar)
	{
		base_vector<T, N> res;
		for (size_t i = 0; i < N; i++)
		{
			res[i] = one[i] * scalar;
		}
		return res;
	}

	template <typename T, size_t N>
	base_vector<T, N> inline operator *( const T & scalar, const base_vector<T, N> & another)
	{
		return another * scalar;
	}

	template <typename T, size_t N>
	base_vector<T, N> inline operator /( const base_vector<T, N> & one, const base_vector<T, N> & another)
	{
		base_vector<T, N> res;
		for (size_t i = 0; i < N; i++)
		{
			res[i] = one[i] / another[i];
		}
		return res;
	}

	template <typename T, size_t N>
	base_vector<T, N> inline operator /( const base_vector<T, N> & one, const T & scalar)
	{
		T recipr = T(1) / scalar;
		base_vector<T, N> res;
		for (size_t i = 0; i < N; i++)
		{
			res[i] = one[i] * recipr;
		}
		return res;
	}

	template <typename T, size_t N>
	base_vector<T, N> inline operator /( const T & scalar, const base_vector<T, N> & another )
	{
		base_vector<T, N> res;
		for (size_t i = 0; i < N; i++)
		{
			res[i] = scalar / another[i];
		}
		return res;
	}

}