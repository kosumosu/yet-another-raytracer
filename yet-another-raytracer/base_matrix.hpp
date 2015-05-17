#pragma once

#include "vector.hpp"
#include "vector_operations.hpp"
#include <cstddef>

namespace math
{
	template <typename T, size_t COLS, size_t ROWS>
	class base_matrix
	{
	public:

		enum { columns = COLS, rows = ROWS };

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

		vector<T, COLS> & operator[](std::size_t index)
		{
			return m_rows[index];
		}

		const vector<T, COLS> & operator[](std::size_t index) const
		{
			return m_rows[index];
		}

		vector<T, ROWS> get_column(std::size_t index) const
		{
			vector<T, ROWS> res;
			for (size_t i = 0; i < ROWS; i++)
			{
				res[i] = m_rows[i][index];
			}
			return res;
		}

	private:
		vector<T, COLS> m_rows[ROWS];
	};
}