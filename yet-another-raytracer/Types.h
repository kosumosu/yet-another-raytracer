#pragma once

#include "new_math.hpp"
#include "math/ray.hpp"
#include "math/bounding_box.hpp"

#include <cstdint>

#define DOUBLE_PRECISION

#if defined (DOUBLE_PRECISION)

using space_real = double;

#else

using space_real = float;
using space_real_precise = double;

#endif

using int_vector2 = math::vector<int, 2>;
using uint_vector2 = math::vector<unsigned int, 2>;

using vector2 = math::vector<space_real, 2>;
using vector3 = math::vector<space_real, 3>;
using vector4 = math::vector<space_real, 4>;
using matrix3 = math::base_matrix<space_real, 3, 3>;
using matrix4 = math::base_matrix<space_real, 4, 4>;
using ray3 = math::ray<space_real, 3>;
using bounding_box3 = math::bounding_box<space_real, 3>;

using color_real = float;
using color_rgb = math::vector<color_real, 3>;
using color_rgbx = math::vector<color_real, 4>;
using color_u8rgb = math::vector<uint8_t, 3>;

constexpr std::size_t UVS_COUNT = 1;
using uvs_t = std::array<vector2, UVS_COUNT>;

using random_int_t = std::uint32_t;