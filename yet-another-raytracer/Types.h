#pragma once

#include "new_math.hpp"
#include "allocation.h"

#define DOUBLE_PRECISION

#if defined (DOUBLE_PRECISION)
typedef math::vector2d vector2;
typedef math::vector3d vector3;
typedef math::vector4d vector4;
typedef math::matrix4d matrix4;
typedef double space_real;
#else
typedef math::vector2f vector2;
typedef math::vector3f vector3;
typedef math::vector4f vector4;
typedef math::matrix4f matrix4;
typedef float space_real;
#endif

typedef math::vector3f color_rgb;
typedef math::vector4f color_rgbx;
typedef float color_real;