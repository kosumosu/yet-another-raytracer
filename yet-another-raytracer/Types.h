#pragma once

#include "new_math.hpp"
#include "ray.hpp"
#include "bounding_box.hpp"

#define DOUBLE_PRECISION

#if defined (DOUBLE_PRECISION)

typedef double space_real;

#else

typedef float space_real;

#endif

typedef math::vector<space_real, 2> vector2;
typedef math::vector<space_real, 3> vector3;
typedef math::vector<space_real, 4> vector4;
typedef math::base_matrix<space_real, 4, 4> matrix4;
typedef math::ray<space_real, 3> ray3;
typedef math::bounding_box<space_real, 3> bounding_box3;

typedef float color_real;
typedef math::vector<color_real, 3> color_rgb;
typedef math::vector<color_real, 4> color_rgbx;