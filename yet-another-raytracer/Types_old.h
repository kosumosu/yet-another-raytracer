#pragma once

#include <glm/glm.hpp>

//#define DOUBLE_PRECISION

#if defined (DOUBLE_PRECISION)
typedef glm::highp_vec3 vector3;
typedef glm::highp_vec4 vector4;
typedef glm::highp_mat4 matrix4;
typedef glm::highp_float space_real;
#else
typedef glm::vec3 vector3;
typedef glm::vec4 vector4;
typedef glm::mat4 matrix4;
typedef glm::mediump_float space_real;
#endif

typedef glm::vec4 color_rgbx;
typedef glm::mediump_float color_real;