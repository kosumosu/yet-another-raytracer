#pragma once

#include "base_matrix.hpp"
#include "base_matrix_operations.hpp"
#include "constants.hpp"

namespace math
{
    template <typename T>
    constexpr base_matrix<T, 3, 3> scale3x3(const vector<T, 3>& vec)
    {
        return base_matrix<T, 3, 3>
        {
            vector<T, 3>{vec[0], 0, 0},
            vector<T, 3>{0, vec[1], 0},
            vector<T, 3>{0, 0, vec[2]}
        };
    }

    template <typename T>
    constexpr base_matrix<T, 3, 3> rotate3x3(const vector<T, 3>& axis, T radians)
    {
        T sinTheta = std::sin(radians);
        T cosTheta = std::cos(radians);

        base_matrix<T, 3, 3> a_star(
            vector<T, 3>{T(0), -axis[2], axis[1]},
            vector<T, 3>{axis[2], T(0), -axis[0]},
            vector<T, 3>{-axis[1], axis[0], T(0)}
        );

        base_matrix<T, 3, 3> a_aT(
            vector<T, 3>{axis[0] * axis[0], axis[0] * axis[1], axis[0] * axis[2]},
            vector<T, 3>{axis[1] * axis[0], axis[1] * axis[1], axis[1] * axis[2]},
            vector<T, 3>{axis[2] * axis[0], axis[2] * axis[1], axis[2] * axis[2]}
        );

        auto res = base_matrix<T, 3, 3>::identity() * cosTheta + a_aT * (1 - cosTheta) + a_star * sinTheta;

        return res;
    }

    template <typename T>
    constexpr base_matrix<T, 3, 3> rotate_degrees3x3(const vector<T, 3>& axis, T degrees)
    {
        T radians = T(math::pi) / T(180) * degrees;
        return rotate(axis, radians);
    }

    template <typename T>
    constexpr base_matrix<T, 3, 3> makeCoordSystemFromVectorZ(vector<T, 3> v1)
    {
        T sign = std::copysign(T(1), v1[2]);
        T a = -1 / (sign + v1[2]);
        T b = v1[0] * v1[1] * a;
        vector<T, 3> v2(
            1 + sign * v1[0] * v1[0] * a,
            sign * b,
            -sign * v1[0]);

        vector<T, 3> v3(
            b,
            sign + v1[1] * v1[1] * a,
            -v1[1]);

        return base_matrix<T, 3, 3>{
            std::move(v2),
            std::move(v3),
            std::move(v1)
        };
    }
}
