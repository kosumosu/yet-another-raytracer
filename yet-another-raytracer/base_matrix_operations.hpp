#pragma once

#include "base_matrix.hpp"
#include "vector_operations.hpp"
#include "helper.hpp"

namespace math
{

	template <typename T, size_t COLS, size_t ROWS>
	bool inline operator == (const base_matrix<T, COLS, ROWS> & one, const base_matrix<T, COLS, ROWS> & another)
	{
		bool equals = true;
		iterate<0, ROWS - 1>([&](size_t i)
		{
			equals &= one[i] == another[i];
		});
		return equals;
	}

	template <typename T, size_t COLS, size_t ROWS>
	bool inline operator != (const base_matrix<T, COLS, ROWS> & one, const base_matrix<T, COLS, ROWS> & another)
	{
		return !(one == another);
	}

	template <typename T, size_t COLS, size_t ROWS>
	base_matrix<T, COLS, ROWS> inline operator * (const base_matrix<T, COLS, ROWS> & one, const T & scalar)
	{
		base_matrix<T, COLS, ROWS> res = base_matrix<T, COLS, ROWS>::identity();
		iterate<0, ROWS - 1>([&](size_t i)
		{
			res[i] = one[i] * scalar;
		});
		return res;
	}

	template <typename T, size_t COLS, size_t ROWS>
	base_matrix<T, COLS, ROWS> inline operator * (const T & scalar, const base_matrix<T, COLS, ROWS> & another)
	{
		return another * scalar;
	}

	template <typename T, size_t COLS, size_t ROWS>
	base_matrix<T, COLS, ROWS> inline operator / (const base_matrix<T, COLS, ROWS> & one, const T & scalar)
	{
		return one * ( T(1) / scalar );
	}


	template <typename T, size_t COLS_ROWS_BOTH, size_t ROWS_ONE, size_t COLS_ANOTHER>
	base_matrix<T, COLS_ANOTHER, ROWS_ONE> inline operator * (const base_matrix<T, COLS_ROWS_BOTH, ROWS_ONE> & one, const base_matrix<T, COLS_ANOTHER, COLS_ROWS_BOTH> & another)
	{
		base_matrix<T, COLS_ANOTHER, ROWS_ONE> res = base_matrix<T, COLS_ANOTHER, ROWS_ONE>::identity();
		for (size_t j = 0; j < ROWS_ONE; j++)
		{
			iterate<0, COLS_ANOTHER - 1>([&](size_t i)
			{
				res[j][i] = math::dot( one[j] , another.get_column(i) );
			});
		}
		return res;
	}

	template <typename T, size_t COLS, size_t ROWS>
	base_matrix<T, COLS, ROWS> inline operator +(const base_matrix<T, COLS, ROWS> & one, const base_matrix<T, COLS, ROWS> & another)
	{
		base_matrix<T, COLS, ROWS> res = base_matrix<T, COLS, ROWS>::identity();
		iterate<0, ROWS - 1>([&](size_t i)
		{
			res[i] = one[i] + another[i];
		});
		return res;
	}

	template <typename T, size_t COLS, size_t ROWS>
	base_matrix<T, COLS, ROWS> inline operator -(const base_matrix<T, COLS, ROWS> & one, const base_matrix<T, COLS, ROWS> & another)
	{
		base_matrix<T, COLS, ROWS> res = base_matrix<T, COLS, ROWS>::identity();
		iterate<0, ROWS - 1>([&](size_t i)
		{
			res[i] = one[i] - another[i];
		});
		return res;
	}


	template <typename T, size_t COLS, size_t ROWS>
	vector<T, ROWS> inline operator * (const base_matrix<T, COLS, ROWS> & one, const vector<T, COLS> & another)
	{
		vector<T, ROWS> res = vector<T, ROWS>::zero();
		iterate<0, ROWS - 1>([&](size_t i)
		{
			res[i] = math::dot(one[i], another);
		});
		return res;
	}
}