#pragma once

#include "base_vector.hpp"
#include "base_vector_operations.hpp"
#include <crtdefs.h>

namespace math
{
	template <typename T, size_t COLS, size_t ROWS>
	class base_matrix
	{
	public:
		// Constructs identity matrix
		base_matrix()
		{
			for (size_t j = 0; j < ROWS; j++)
			{
				for (size_t i = 0; i < COLS; i++)
				{
					m_rows[j][i] = i == j ? T(1) : T(0);
				}
			}
		}

		template<typename TOther>
		base_matrix(const base_matrix<TOther, COLS, ROWS> & other)
		{
			for (size_t j = 0; j < ROWS; j++)
			{
				for (size_t i = 0; i < COLS; i++)
				{
					m_rows[j][i] = other[j][i];
				}
			}
		}

		size_t num_rows() const { return ROWS; }
		size_t num_cols() const { return COLS; }

		base_vector<T, COLS> & operator[](std::size_t index)
		{
			return m_rows[index];
		}

		const base_vector<T, COLS> & operator[](std::size_t index) const
		{
			return m_rows[index];
		}

		base_vector<T, ROWS> get_column(std::size_t index) const
		{
			base_vector<T, ROWS> res;
			for (size_t i = 0; i < ROWS; i++)
			{
				res[i] = m_rows[i][index];
			}
			return res;
		}

	private:
		base_vector<T, COLS> m_rows[ROWS];
	};
}