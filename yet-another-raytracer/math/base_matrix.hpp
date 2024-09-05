#pragma once

#include "vector.hpp"

namespace math
{
	template <typename T, std::size_t COLS, std::size_t ROWS>
	class base_matrix
	{
		vector<T, COLS> m_rows[ROWS];

		struct internal_tag {};

	public:

		constexpr static std::size_t columns() { return COLS; }
		constexpr static std::size_t rows() { return ROWS; }

		constexpr static base_matrix<T, COLS, ROWS> uniform_scale(const T & scale) { return generate_matrix(scale, std::make_index_sequence<ROWS>()); }
		constexpr static base_matrix<T, COLS, ROWS> identity() { return uniform_scale(1); }
		constexpr static base_matrix<T, COLS, ROWS> zero() { return uniform_scale(0); }


		// Constructs identity matrix
		//base_matrix()
		//{
		//	for (std::size_t j = 0; j < ROWS; j++)
		//	{
		//		iterate<0, COLS - 1>([&](std::size_t i)
		//		{
		//			m_rows[j][i] = i == j ? T(1) : T(0);
		//		});
		//	}
		//}

		// template <typename TOther>
		// constexpr base_matrix(const base_matrix<TOther, COLS, ROWS>& other)
		// 	: base_matrix(internal_tag(), other, std::make_index_sequence<ROWS>())
		// {
		// }
		//
		// constexpr base_matrix(base_matrix<T, COLS, ROWS> && other) noexcept
		// 	: m_rows{ std::move(other.m_rows) }
		// {
		// }

		template <typename TFirst, typename... TRows/*, class = std::enable_if_t<sizeof...(TRows) == ROWS>*/>
		constexpr explicit base_matrix(vector<TFirst, COLS> first, vector<TRows, COLS> ... rows)
			: m_rows{ std::move(first), std::move(rows)... }
		{
		}


		vector<T, COLS> & operator[](std::size_t index)
		{
			return m_rows[index];
		}

		constexpr const vector<T, COLS> & operator[](std::size_t index) const
		{
			return m_rows[index];
		}

		constexpr vector<T, ROWS> get_column(std::size_t index) const
		{
			return get_column(index, std::make_index_sequence<ROWS>());
		}

	private:

		template<typename TOther, std::size_t... Indices>
		constexpr base_matrix(internal_tag, const base_matrix<TOther, COLS, ROWS> & other, std::index_sequence<Indices...>)
			: m_rows{ other[Indices]... }
		{
			
		}

		template<std::size_t... Indices>
		constexpr static base_matrix<T, COLS, ROWS> generate_matrix(const T & value, std::index_sequence<Indices...>)
		{
			return base_matrix<T, COLS, ROWS>(generate_row(Indices, value, std::make_index_sequence<COLS>())...);
		}

		template<std::size_t... Indices>
		constexpr static vector<T, COLS> generate_row(std::size_t position, const T & value, std::index_sequence<Indices...>)
		{
			return vector<T, COLS>((Indices == position ? value : T(0))...);
		}

		template<std::size_t... RowIndices>
		constexpr vector<T, ROWS> get_column(std::size_t index, std::index_sequence<RowIndices...>) const
		{
			return vector<T, ROWS>{ m_rows[RowIndices][index]... };
		}
	};
}