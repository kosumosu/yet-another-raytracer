#pragma once

// fucking windows.h
#undef max
#undef min

#include "vector.hpp"
#include "helper.hpp"
#include <algorithm>


namespace math
{
	template <typename T, std::size_t N>
	constexpr bool operator ==( const vector<T, N> & one, const vector<T, N> & another)
	{
		bool equals = true;
		iterate<0, N - 1>([&](std::size_t i)
		{
			equals &= one[i] == another[i];
		});
		return equals;
	}

	template <typename T, std::size_t N>
	constexpr bool operator !=( const vector<T, N> & one, const vector<T, N> & another)
	{
		return !(one == another);
	}

	template <typename T, std::size_t N>
	constexpr vector<T, N> operator +( const vector<T, N> & one, const vector<T, N> & another)
	{
		vector<T, N> res = vector<T, N>::zero();
		iterate<0, N - 1>([&](std::size_t i)
		{
			res[i] = one[i] + another[i];
		});
		return res;
	}

	template <typename T, std::size_t N>
	vector<T, N> & operator +=( vector<T, N> & one, const vector<T, N> & another)
	{
		iterate<0, N - 1>([&](std::size_t i)
		{
			one[i] += another[i];
		});
		return one;
	}

	template <typename T, std::size_t N>
	constexpr vector<T, N> operator -( const vector<T, N> & one, const vector<T, N> & another)
	{
		vector<T, N> res = vector<T, N>::zero();
		iterate<0, N - 1>([&](std::size_t i)
		{
			res[i] = one[i] - another[i];
		});
		return res;
	}

	template <typename T, std::size_t N>
	vector<T, N> & operator -=( vector<T, N> & one, const vector<T, N> & another)
	{
		iterate<0, N - 1>([&](std::size_t i)
		{
			one[i] -= another[i];
		});
		return one;
	}

	template <typename T, std::size_t N>
	vector<T, N> operator -( const vector<T, N> & one)
	{
		vector<T, N> res = vector<T, N>::zero();
		iterate<0, N - 1>([&](std::size_t i)
		{
			res[i] = -one[i];
		});
		return res;
	}

	template <typename T1, typename T2, std::size_t N>
	constexpr auto operator *(const vector<T1, N>& one, const vector<T2, N>& another)
	{
		using TResult = decltype(one[0] * another[0]);
		auto res = vector<TResult, N>::zero();
		iterate<0, N - 1>([&](std::size_t i)
			{
				res[i] = one[i] * another[i];
			});
		return res;
	}

	template <typename T, std::size_t N>
	vector<T, N> & operator *=(vector<T, N> & one, const vector<T, N> & another)
	{
		iterate<0, N - 1>([&](std::size_t i)
		{
			one[i] *= another[i];
		});
		return one;
	}

	template <typename T1, typename T2, std::size_t N>
	constexpr auto operator *(const vector<T1, N>& one, const T2& scalar)
	{
		using TResult = decltype(one[0] * scalar);
		auto res = vector<TResult, N>::zero();
		iterate<0, N - 1>([&](std::size_t i)
			{
				res[i] = one[i] * scalar;
			});
		return res;
	}

	template <typename T, std::size_t N>
	vector<T, N> & operator *=(vector<T, N> & one, const T & scalar)
	{
		iterate<0, N - 1>([&](std::size_t i)
		{
			one[i] *= scalar;
		});
		return one;
	}

	template <typename T1, typename T2, std::size_t N>
	constexpr auto operator *( const T1 & scalar, const vector<T2, N> & another)
	{
		using TResult = decltype(scalar * another[0]);
		auto res = vector<TResult, N>::zero();
		iterate<0, N - 1>([&](std::size_t i)
			{
				res[i] = scalar * another[i];
			});
		return res;
	}


	template <typename T1, typename T2, std::size_t N>
	constexpr auto operator /(const vector<T1, N>& one, const vector<T2, N>& another)
	{
		using TResult = decltype(one[0] / another[0]);
		auto res = vector<TResult, N>::zero();
		iterate<0, N - 1>([&](std::size_t i)
			{
				res[i] = one[i] / another[i];
			});
		return res;
	}

	template <typename T1, typename T2, std::size_t N>
	constexpr auto operator /(const vector<T1, N>& one, const T2& scalar)
	{
		using TResult = decltype(one[0] / scalar);
		auto res = vector<TResult, N>::zero();
		iterate<0, N - 1>([&](std::size_t i)
			{
				res[i] = one[i] / scalar;
			});
		return res;
	}

	template <typename T1, typename T2, std::size_t N>
	constexpr auto operator /(const T1& scalar, const vector<T2, N>& another)
	{
		using TResult = decltype(scalar * another[0]);
		auto res = vector<TResult, N>::zero();
		iterate<0, N - 1>([&](std::size_t i)
			{
				res[i] = scalar / another[i];
			});
		return res;
	}

	template <typename T1, typename T2, std::size_t N>
	constexpr auto operator %(const vector<T1, N>& one, const vector<T2, N>& another)
	{
		using TResult = decltype(one[0] % another[0]);
		auto res = vector<TResult, N>::zero();
		iterate<0, N - 1>([&](std::size_t i)
			{
				res[i] = one[i] % another[i];
			});
		return res;
	}

}