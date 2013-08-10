#pragma once

#include "base_vector3.hpp"

#include <algorithm>

namespace math
{

	template <typename T>
	base_vector<T, 3> inline cross( const base_vector<T, 3> & one, const base_vector<T, 3> & other)
	{
		return base_vector<T, 3>
			(
			one.y() * other.z() - one.z() * other.y(),
			one.z() * other.x() - one.x() * other.z(),
			one.x() * other.y() - one.y() * other.x()
			);
	}

}