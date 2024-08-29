#pragma once

//#define MATH_ENABLE_SSE

#include "math/vector.hpp"
#include "math/vector_operations.hpp"
#include "math/vector_functions.hpp"
//#include "base_vector3.hpp"
#include "math/vector3_operations.hpp"
#include "math/vector3_functions.hpp"
//#include "vector4.hpp"
#if defined(MATH_ENABLE_SSE)
#include "math/sse_float_vector3.hpp"
#include "math/sse_float_vector3_operations.hpp"
#include "math/sse_float_vector4.hpp"
#include "math/sse_float_vector4_operations.hpp"
#endif
#include "math/base_matrix_operations.hpp"
#include "math/base_matrix_functions.hpp"
#include "math/base_matrix4x4_transfrom.hpp"
#include "math/random.hpp"