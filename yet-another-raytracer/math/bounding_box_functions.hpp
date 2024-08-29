#pragma once

#include "bounding_box.hpp"
#include "vector_functions.hpp"

namespace math
{
	template <typename TSpace>
	TSpace get_surface_area(const bounding_box<TSpace, 3> & box)
	{
		auto diff = box.max_corner() - box.min_corner();
		TSpace half_area = TSpace(0);
		return TSpace(2.0) * (diff[0] * diff[1] + diff[0] * diff[2] + diff[1] * diff[2]);
	}
}