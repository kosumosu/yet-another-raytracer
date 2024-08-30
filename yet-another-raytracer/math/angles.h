#pragma once

#include <numbers>

namespace math
{
    template <class T>
    constexpr T deg_to_rad(const T& deg_angle)
    {
        return deg_angle * std::numbers::pi_v<T> / T(180.0);
    }

    template <class T>
    constexpr T rad_to_deg(const T& rad_angle)
    {
        return rad_angle * std::numbers::inv_pi_v<T> * T(180.0);
    }
}
