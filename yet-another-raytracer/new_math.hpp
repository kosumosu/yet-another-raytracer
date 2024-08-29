#pragma once

//#define MATH_ENABLE_SSE

#include "vector.hpp"
#include "vector_operations.hpp"
#include "vector_functions.hpp"
//#include "base_vector3.hpp"
#include "vector3_operations.hpp"
#include "vector3_functions.hpp"
//#include "vector4.hpp"
#if defined(MATH_ENABLE_SSE)
#include "sse_float_vector3.hpp"
#include "sse_float_vector3_operations.hpp"
#include "sse_float_vector4.hpp"
#include "sse_float_vector4_operations.hpp"
#endif
#include "base_matrix_operations.hpp"
#include "base_matrix_functions.hpp"
#include "base_matrix4x4_transfrom.hpp"
#include "random.hpp"