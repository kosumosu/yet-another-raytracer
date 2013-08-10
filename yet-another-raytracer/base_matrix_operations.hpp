#pragma once

#include "base_matrix.hpp"
#include "base_vector_operations.hpp"

namespace math
{

	template <typename T, size_t COLS, size_t ROWS>
	bool inline operator == (const base_matrix<T, COLS, ROWS> & one, const base_matrix<T, COLS, ROWS> & another)
	{
		for (size_t i = 0; i < ROWS; i++)
		{
			if (one[i] != another[i])
				return false;
		}
		return true;
	}

	template <typename T, size_t COLS, size_t ROWS>
	bool inline operator != (const base_matrix<T, COLS, ROWS> & one, const base_matrix<T, COLS, ROWS> & another)
	{
		return !(one == another);
	}

	template <typename T, size_t COLS, size_t ROWS>
	base_matrix<T, COLS, ROWS> inline operator * (const base_matrix<T, COLS, ROWS> & one, const T & scalar)
	{
		base_matrix<T, COLS, ROWS> res;
		for (size_t i = 0; i < ROWS; i++)
		{
			res[i] = one[i] * scalar;
		}
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
		base_matrix<T, COLS_ANOTHER, ROWS_ONE> res;
		for (size_t j = 0; j < ROWS_ONE; j++)
		{
			for (size_t i = 0; i < COLS_ANOTHER; i++)
			{
				res[j][i] = math::dot( one[j] , another.get_column(i) );
			}
		}
		return res;
	}

	template <typename T, size_t COLS, size_t ROWS>
	base_matrix<T, COLS, ROWS> inline operator +(const base_matrix<T, COLS, ROWS> & one, const base_matrix<T, COLS, ROWS> & another)
	{
		base_matrix<T, COLS, ROWS> res;
		for (size_t i = 0; i < ROWS; i++)
		{
			res[i] = one[i] + another[i];
		}
		return res;
	}

	template <typename T, size_t COLS, size_t ROWS>
	base_matrix<T, COLS, ROWS> inline operator -(const base_matrix<T, COLS, ROWS> & one, const base_matrix<T, COLS, ROWS> & another)
	{
		base_matrix<T, COLS, ROWS> res;
		for (size_t i = 0; i < ROWS; i++)
		{
			res[i] = one[i] - another[i];
		}
		return res;
	}


	template <typename T, size_t COLS, size_t ROWS>
	base_vector<T, ROWS> inline operator * (const base_matrix<T, COLS, ROWS> & one, const base_vector<T, COLS> & another)
	{
		base_vector<T, ROWS> res;
		for (size_t i = 0; i < ROWS; i++)
		{
			res[i] = math::dot( one[i] , another );
		}
		return res;
	}
}