#pragma once

#include "vector.hpp"
#include "helper.hpp"

namespace math
{

	template <typename T>
	vector<T, 3> cross( const vector<T, 3> & one, const vector<T, 3> & other) {
		return vector<T, 3>
			(
			diffOfProducts(one[1], other[2], one[2], other[1]),
			diffOfProducts(one[2], other[0], one[0], other[2]),
			diffOfProducts(one[0], other[1], one[1], other[0])
			);
	}

}