#pragma once

#include "vector.hpp"
#include <nmmintrin.h>
#include <crtdefs.h>

#define REINTERPRET_CAST_INT_TO_FLOAT( integer ) (  *(reinterpret_cast<float *>(&(integer))) )

/************************************************************************/
/* SSE vector implementation. Think of it as unaligned.                 */
/************************************************************************/
namespace math
{
	template<>
	class __declspec(align(16)) vector<float, 3>
	{
	public:
		vector()
		{
			m_values = _mm_setzero_ps();
		}

		vector(const float & value)
		{
			m_values = _mm_set_ps(float(0), value, value, value);
		}

		vector(const float values[3])
		{
			m_values = _mm_set_ps(float(0), values[2], values[1], values[0]);
		}

		vector(const __m128 & xmm)
		{
			m_values = xmm;
		}

		template<typename TOther>
		vector(const vector<TOther, 3> & other)
		{
			m_values = _mm_set_ps(float(0), other[2], other[1], other[0]);
		}

		vector(const vector<float, 3> & other)
		{
			m_values = other.values();
		}

		vector(const float & x, const float & y, const float & z)
		{
			m_values = _mm_set_ps(float(0), z, y, x);
		}

		std::size_t size() const { return 3; }


		float & operator[](std::size_t index)
		{
			return m_values.m128_f32[index];
		}

		float operator[](std::size_t index) const
		{
			return m_values.m128_f32[index];

			// WARNING : AVX
			//return _mm_cvtss_f32(_mm_permutevar_ps(m_values, _mm_cvtsi32_si128((int)index)));

			//float res;
			//switch (index)
			//{
			//case 0:
			//	_MM_EXTRACT_FLOAT(res, m_values, 0);
			//	break;
			//case 1:
			//	_MM_EXTRACT_FLOAT(res, m_values, 1);
			//	break;
			//case 2:
			//	_MM_EXTRACT_FLOAT(res, m_values, 3);
			//	break;
			//case 3:
			//	_MM_EXTRACT_FLOAT(res, m_values, 4);
			//	break;
			//}
			//return res;
		}

		//float operator[](std::size_t index) const
		//{
		//	return m_values.m128_f32[index];
		//}

		vector<float, 2> reduce() const
		{
			vector<float, 2> res;
			res[0] = x();
			res[1] = y();
			return res;
		}

		float & x() { return (*this)[0]; }
		float x() const { return (*this)[0]; }

		float & y() { return (*this)[1]; }
		float y() const { return (*this)[1]; }

		float & z() { return (*this)[2]; }
		float z() const { return (*this)[2]; }

		const __m128 & values() const { return m_values; }
		__m128 & values() { return m_values; }

	private:
		__m128 m_values;
	};


}