#pragma once

#include "ray.hpp"
#include "base_matrix.hpp"
#include "base_matrix_operations.hpp"

namespace math
{
	template <typename TSpace, std::size_t ROWS, std::size_t COLUMNS>
	ray<TSpace, ROWS> transform(const ray<TSpace, COLUMNS> & inputRay, const base_matrix<TSpace, COLUMNS, ROWS> & matrix)
	{
		auto origin = matrix * inputRay.origin();
		auto direction = matrix * inputRay.direction();
		return ray<TSpace, ROWS>(origin, direction);
	}

	template <typename TSpace>
	ray<TSpace, 3> transform3by4x4(const ray<TSpace, 3> & inputRay, const base_matrix<TSpace, 4, 4> & matrix)
	{
		auto origin = (matrix * vector<TSpace, 4>(inputRay.origin(), TSpace(1.0))).reduce();
		auto direction = (matrix * vector<TSpace, 4>(inputRay.direction(), TSpace(0.0))).reduce();
		return ray<TSpace, 3>(origin, direction);
	}

	template <typename TSpace, std::size_t DIMENSIONS>
	ray<TSpace, DIMENSIONS> MoveOriginAlongDirection(const ray<TSpace, DIMENSIONS> & inputRay, const TSpace & distance)
	{
		return ray<TSpace, DIMENSIONS>(inputRay.origin() + inputRay.direction() * distance, inputRay.direction(), inputRay.inversed_direction());
	}
}