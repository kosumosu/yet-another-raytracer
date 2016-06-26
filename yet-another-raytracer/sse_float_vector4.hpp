#pragma once

#include "allocation.h"
#include "vector.hpp"
#include "sse_float_vector3.hpp"
#include <nmmintrin.h>
#include <crtdefs.h>

/************************************************************************/
/* SSE vector implementation. Think of it as unaligned.                 */
/************************************************************************/
namespace math
{
	template<>
	class vector<float, 4>
	{
	public:
		vector()
		{
			m_values = _mm_setzero_ps();
		}

		vector(const float & value)
		{
			m_values = _mm_set_ps(value, value, value, value);
		}

		vector(const float values[4])
		{
			m_values = _mm_set_ps(values[3], values[2], values[1], values[0]);
		}

		vector(const __m128 & xmm)
		{
			m_values = xmm;
		}

		template<typename TOther>
		vector(const vector<TOther, 4> & other)
		{
			m_values = _mm_set_ps(other[3], other[2], other[1], other[0]);
		}

		template<typename TOther>
		vector(const vector<TOther, 3> & other, const float & w)
		{
			m_values = _mm_set_ps(w, other[2], other[1], other[0]);
		}


		vector(const vector<float, 4> & other)
		{
			m_values = other.values();
		}

		vector(const vector<float, 3> & other)
		{
			m_values = other.values();
		}

		__declspec(noinline)
		vector(const vector<float, 3> & other, const float & w)
		{
			m_values = _mm_blend_ps(other.values(), _mm_set1_ps(w), 0x8);
		}

		vector(const float & x, const float & y, const float & z, const float & w)
		{
			m_values = _mm_set_ps(w, z, y, x);
		}

		size_t size() const { return 4; }

		float & operator[](std::size_t index)
		{
			return m_values.m128_f32[index];
		}

		const float & operator[](std::size_t index) const
		{
			return m_values.m128_f32[index];
		}

		vector<float, 3> reduce() const
		{
			__m128 reg = m_values;
			return vector<float, 3>(_mm_blend_ps(m_values, _mm_setzero_ps(), 0x8));
		}

		float & x() { return (*this)[0]; }
		const float & x() const { return (*this)[0]; }

		float & y() { return (*this)[1]; }
		const float & y() const { return (*this)[1]; }

		float & z() { return (*this)[2]; }
		const float & z() const { return (*this)[2]; }

		float & w() { return (*this)[3]; }
		const float & w() const { (*this)[3]; }

		const __m128 & values() const { return m_values; }
		__m128 & values() { return m_values; }

	private:
		__m128 m_values;
	};


}