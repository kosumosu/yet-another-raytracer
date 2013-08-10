#pragma once

#include "base_vector.hpp"
#include <crtdefs.h>
/************************************************************************/
/* Generic 3d vector implementation                                     */
/************************************************************************/
namespace math
{
	template<typename T>
	class base_vector<T, 3>
	{
	public:
		base_vector()
		{
			m_values[0] = T(0);
			m_values[1] = T(0);
			m_values[2] = T(0);
		}

		base_vector(const T & value)
		{
			m_values[0] = T(value);
			m_values[1] = T(value);
			m_values[2] = T(value);
		}

		base_vector(const T values[3])
		{
			m_values[0] = values[0];
			m_values[1] = values[1];
			m_values[2] = values[2];
		}

		template<typename TOther, size_t N>
		base_vector(const base_vector<TOther, N> & other)
		{
			static_assert(N >= 3, "size of original vector can't be less than constructing one");
			m_values[0] = T(other[0]);
			m_values[1] = T(other[1]);
			m_values[2] = T(other[2]);
		}

		//template<typename TOther>
		//base_vector(const base_vector<TOther, 3> & other)
		//{
		//	m_values[0] = other[0];
		//	m_values[1] = other[1];
		//	m_values[2] = other[2];
		//}

		base_vector(const T & x, const T & y, const T & z)
		{
			m_values[0] = x;
			m_values[1] = y;
			m_values[2] = z;
		}

		size_t size() const { return 3; }

		T & operator[](std::size_t index)
		{
			return m_values[index];
		}

		const T & operator[](std::size_t index) const
		{
			return m_values[index];
		}

		base_vector<T, 2> reduce() const
		{
			return base_vector<T, 2>(m_values);
		}

		T & x() { return m_values[0]; }
		const T & x() const { return m_values[0]; }

		T & y() { return m_values[1]; }
		const T & y() const { return m_values[1]; }

		T & z() { return m_values[2]; }
		const T & z() const { return m_values[2]; }

	private:
		// we don't mess with alignment
		T m_values[3];
	};
}