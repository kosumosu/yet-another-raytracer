#pragma once

//#include "base_vector3.hpp"
//#include "base_matrix4x4.hpp"
#include "base_matrix.hpp"
#include "base_matrix_operations.hpp"
#include "constants.hpp"

namespace math
{
	template <typename T>
	constexpr base_matrix<T, 4, 4> translate(const vector<T, 3> & vec)
	{
		return base_matrix<T, 4, 4>
		{
			vector<T, 4> {1, 0, 0, vec[0]},
			vector<T, 4> {0, 1, 0, vec[1]},
			vector<T, 4> {0, 0, 1, vec[2]},
			vector<T, 4> {0, 0, 0, 1}
		};
	}

	template <typename T>
	constexpr base_matrix<T, 4, 4> scale(const vector<T, 3> & vec)
	{
		return base_matrix<T, 4, 4>
		{
			vector<T, 4> {vec[0], 0, 0, 0},
			vector<T, 4> {0, vec[1], 0, 0},
			vector<T, 4> {0, 0, vec[2], 0},
			vector<T, 4> {0, 0, 0, 1}
		};
	}

	template <typename T>
	base_matrix<T, 4, 4> rotate(const vector<T, 3> & axis, T radians)
	{
		T sinTheta = std::sin(radians);
		T cosTheta = std::cos(radians);

		base_matrix<T, 4, 4> a_star(
			vector<T, 4>{T(0), -axis[2], axis[1], T(0)},
			vector<T, 4>{axis[2], T(0), -axis[0], T(0)},
			vector<T, 4>{-axis[1], axis[0], T(0), T(0)},
			vector<T, 4>{T(0), T(0), T(0), T(0)}
		);

		base_matrix<T, 4, 4> a_aT(
			vector<T, 4>{axis[0] * axis[0], axis[0] * axis[1], axis[0] * axis[2], T(0)},
			vector<T, 4>{axis[1] * axis[0], axis[1] * axis[1], axis[1] * axis[2], T(0)},
			vector<T, 4>{axis[2] * axis[0], axis[2] * axis[1], axis[2] * axis[2], T(0)},
			vector<T, 4>{T(0), T(0), T(0), T(0)}
		);

		auto res = base_matrix<T, 4, 4>::identity() * cosTheta + a_aT * (1 - cosTheta) + a_star * sinTheta;
		res[3][3] = T(1);
		return res;
	}

	template <typename T>
	base_matrix<T, 4, 4> rotate_degrees(const vector<T, 3> & axis, T degrees)
	{
		T radians = T(math::pi) / T(180) * degrees;
		return rotate(axis, radians);
	}
}
