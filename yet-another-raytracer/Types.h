#pragma once

#include "new_math.hpp"
#include "ray.hpp"
#include "bounding_box.hpp"

#define DOUBLE_PRECISION

#if defined (DOUBLE_PRECISION)

using space_real = double;

#else

using space_real = float;

#endif

using vector2 = math::vector<space_real, 2>;
using vector3 = math::vector<space_real, 3>;
using vector4 = math::vector<space_real, 4>;
using matrix4 = math::base_matrix<space_real, 4, 4>;
using ray3 = math::ray<space_real, 3>;
using bounding_box3 = math::bounding_box<space_real, 3>;

using color_real = float;
using color_rgb = math::vector<color_real, 3>;
using color_rgbx = math::vector<color_real, 4>;

constexpr size_t UVS_COUNT = 1;
using uvs_t = std::array<vector2, UVS_COUNT>;

// to workaround upper bound issue with STL PRNG
template <typename T>
constexpr T upperRandomBound() { return std::nextafter(T(1.0), T(0.0)); } // made it a func to pleasure resharper.