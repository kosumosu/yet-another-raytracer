#pragma once

#include <cstddef>
/************************************************************************/
/* Generic n-d vector implementation                                    */
/************************************************************************/
namespace math
{
	template<typename T, size_t N>
	class base_vector
	{
	public:
		base_vector()
		{
			for (size_t i = 0; i < N; i++)
			{
				m_values[i] = T(0);
			}
		}

		base_vector(const T & value)
		{
			for (size_t i = 0; i < N; i++)
			{
				m_values[i] = value;
			}
		}

		base_vector(const T values[N])
		{
			for (size_t i = 0; i < N; i++)
			{
				m_values[i] = values[i];
			}
		}

		template<typename TOther>
		base_vector(const base_vector<TOther, N> & other)
		{
			for (size_t i = 0; i < N; i++)
			{
				m_values[i] = other[i];
			}
		}

		template<typename TOther>
		base_vector(const base_vector<TOther, N - 1> & other, const T & last_value)
		{
			for (size_t i = 0; i < N - 1; i++)
			{
				m_values[i] = other[i];
			}
			m_values[N - 1] = last_value;
		}

		size_t size() const { return N; }

		T & operator[](std::size_t index)
		{
			return m_values[index];
		}

		const T & operator[](std::size_t index) const
		{
			return m_values[index];
		}

		base_vector<T, N - 1> reduce() const
		{
			return base_vector<T, N - 1>(m_values);
		}

	private:
		// we don't mess with alignment
		T m_values[N];
	};


}