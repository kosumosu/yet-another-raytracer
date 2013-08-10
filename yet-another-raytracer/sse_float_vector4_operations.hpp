#pragma once

#undef max
#undef min

#include "sse_float_vector4.hpp"

#include <nmmintrin.h>
#include <algorithm>

namespace math
{
	bool inline operator ==( const vector<float, 4> & one, const vector<float, 4> & other)
	{
		return 0xF ==_mm_movemask_ps(_mm_cmpeq_ps(one.values(), other.values()));
	}

	bool inline operator !=( const vector<float, 4> & one, const vector<float, 4> & other)
	{
		return 0xF !=_mm_movemask_ps(_mm_cmpeq_ps(one.values(), other.values()));
	}

	vector<float, 4> inline operator +( const vector<float, 4> & one, const vector<float, 4> & other)
	{
		return vector<float, 4>(_mm_add_ps(one.values(), other.values()));
	}

	vector<float, 4> inline operator +=( vector<float, 4> & one, const vector<float, 4> & other)
	{
		one.values() = _mm_add_ps(one.values(), other.values());
		return one;
	}

	vector<float, 4> inline operator -( const vector<float, 4> & one, const vector<float, 4> & other)
	{
		return vector<float, 4>(_mm_sub_ps(one.values(), other.values()));
	}

	vector<float, 4> inline operator -=( vector<float, 4> & one, const vector<float, 4> & other)
	{
		one.values() = _mm_sub_ps(one.values(), other.values());
		return one;
	}

	vector<float, 4> inline operator -( const vector<float, 4> & one)
	{
		__m128 sign_mask = _mm_castsi128_ps(_mm_set1_epi32(0x80000000));
		return vector<float, 4>(_mm_xor_ps(sign_mask, one.values()));
	}

	vector<float, 4> inline operator *( const vector<float, 4> & one, const vector<float, 4> & other)
	{
		return vector<float, 4>(_mm_mul_ps(one.values(), other.values()));
	}

	vector<float, 4> inline operator *( const vector<float, 4> & one, const float & scalar)
	{
		return vector<float, 4>(_mm_mul_ps(one.values(), _mm_set1_ps(scalar)));
	}

	vector<float, 4> inline operator *( const float & scalar, const vector<float, 4> & other)
	{
		return other * scalar;
	}

	vector<float, 4> inline operator /( const vector<float, 4> & one, const vector<float, 4> & other)
	{
		return vector<float, 4>(_mm_div_ps(one.values(), other.values()));
	}

	vector<float, 4> inline operator /( const vector<float, 4> & one, const float & scalar)
	{
		return vector<float, 4>(_mm_div_ps(one.values(), _mm_set1_ps(scalar)));
	}

	vector<float, 4> inline operator /( const float & scalar, const vector<float, 4> & other )
	{
		return vector<float, 4>(_mm_div_ps(_mm_set1_ps(scalar), other.values()));
	}

	float inline dot( const vector<float, 4> & one, const vector<float, 4> & other)
	{
		return _mm_cvtss_f32(_mm_dp_ps(one.values(), other.values(), 0xF1));
	}

	vector<float, 4> inline dot_vec( const vector<float, 4> & one, const vector<float, 4> & other)
	{
		return vector<float, 4>(_mm_dp_ps(one.values(), other.values(), 0xFF));
	}

	float inline length2( const vector<float, 4> & one )
	{
		return dot(one, one);
	}

	float inline length( const vector<float, 4> & one )
	{
		return _mm_cvtss_f32(_mm_sqrt_ss(_mm_dp_ps(one.values(), one.values(), 0xF1)));
	}

	// Not guaranteed to be precise but may be faster.
	float inline fast_length( const vector<float, 4> & one )
	{
		return length(one);
	}

	float inline inv_length( const vector<float, 4> & one )
	{
		return float(1) / _mm_cvtss_f32(_mm_sqrt_ss(_mm_dp_ps(one.values(), one.values(), 0xF1)));
	}

	// Not guaranteed to be precise but may be faster.
	float inline fast_inv_length( const vector<float, 4> & one )
	{
		return _mm_cvtss_f32(_mm_rsqrt_ss(_mm_dp_ps(one.values(), one.values(), 0xF1)));
	}

	vector<float, 4> inline normalize( const vector<float, 4> & one )
	{
		return vector<float, 4>(_mm_div_ps(one.values(), _mm_sqrt_ps(_mm_dp_ps(one.values(), one.values(), 0xFF))));
	}

	vector<float, 4> inline fast_normalize( const vector<float, 4> & one )
	{
		return vector<float, 4>(_mm_mul_ps(one.values(), _mm_rsqrt_ps(_mm_dp_ps(one.values(), one.values(), 0xFF))));
	}

	vector<float, 4> inline reciprocal( const vector<float, 4> & one )
	{
		return float(1) / one;
	}

	// Not guaranteed to be precise but may be faster.
	vector<float, 4> inline fast_reciprocal( const vector<float, 4> & one )
	{
		return vector<float, 4>(_mm_rcp_ps(one.values()));
	}


	vector<float, 4> inline max( const vector<float, 4> & one, const vector<float, 4> & other)
	{
		return vector<float, 4>(_mm_max_ps(one.values(), other.values()));
	}

	vector<float, 4> inline min( const vector<float, 4> & one, const vector<float, 4> & other)
	{
		return vector<float, 4>(_mm_min_ps(one.values(), other.values()));
	}

	vector<float, 4> inline abs( const vector<float, 4> & one)
	{
		__m128 sign_mask = _mm_castsi128_ps(_mm_set_epi32(0x80000000, 0x80000000, 0x80000000, 0x80000000));
		return vector<float, 4>(_mm_andnot_ps(sign_mask, one.values()));
	}
}