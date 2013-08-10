#pragma once

#include "base_vector.hpp"
#include <crtdefs.h>
/************************************************************************/
/* Generic 4d vector implementation                                     */
/************************************************************************/
namespace math
{
	template<typename T>
	class base_vector<T, 4>
	{
	public:
		base_vector()
		{
			m_values[0] = T(0);
			m_values[1] = T(0);
			m_values[2] = T(0);
			m_values[3] = T(0);
		}

		base_vector(const T & value)
		{
			m_values[0] = T(value);
			m_values[1] = T(value);
			m_values[2] = T(value);
			m_values[3] = T(value);
		}

		base_vector(const T values[4])
		{
			m_values[0] = values[0];
			m_values[1] = values[1];
			m_values[2] = values[2];
			m_values[3] = values[3];
		}

		template<typename TOther>
		base_vector(const base_vector<TOther, 4> & other)
		{
			m_values[0] = T(other[0]);
			m_values[1] = T(other[1]);
			m_values[2] = T(other[2]);
			m_values[3] = T(other[3]);
		}

		template<typename TOther>
		base_vector(const base_vector<TOther, 3> & other, const T & w)
		{
			m_values[0] = T(other[0]);
			m_values[1] = T(other[1]);
			m_values[2] = T(other[2]);
			m_values[3] = w;
		}

		base_vector(const T & x, const T & y, const T & z, const T & w)
		{
			m_values[0] = x;
			m_values[1] = y;
			m_values[2] = z;
			m_values[3] = w;
		}

		size_t size() const { return 4; }

		T & operator[](std::size_t index)
		{
			return m_values[index];
		}

		const T & operator[](std::size_t index) const
		{
			return m_values[index];
		}

		base_vector<T, 3> reduce() const
		{
			return base_vector<T, 3>(m_values);
		}

		T & x() { return m_values[0]; }
		const T & x() const { return m_values[0]; }

		T & y() { return m_values[1]; }
		const T & y() const { return m_values[1]; }

		T & z() { return m_values[2]; }
		const T & z() const { return m_values[2]; }

		T & w() { return m_values[3]; }
		const T & w() const { return m_values[3]; }


	private:
		// we don't mess with alignment
		T m_values[4];
	};
}