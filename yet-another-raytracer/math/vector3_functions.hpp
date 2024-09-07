#pragma once

#include "vector.hpp"

#include <cmath>

namespace math
{
    template <typename T>
    constexpr vector<T, 3> from_angles(const T& azimuth, const T& elevation)
    {
        const auto cos_azimuth = std::cos(azimuth);
        const auto sin_azimuth = std::sin(azimuth);

        const auto cos_elev = std::cos(elevation);
        const auto sin_elev = std::sin(elevation);

        return vector<T, 3>
        {
            cos_azimuth * cos_elev,
            sin_azimuth * cos_elev,
            sin_elev
        };
    }

    template <typename T>
    constexpr vector<T, 3> from_theta_phi_towards_x(const T& sin_theta, const T& cos_theta, const T& phi)
    {
        return vector<T, 3>
        {
            std::clamp(cos_theta, -T(1), T(1)),
            std::clamp(sin_theta, -T(1), T(1)) * std::cos(phi),
            std::clamp(sin_theta, -T(1), T(1)) * std::sin(phi)
        };
    }

    template <typename T>
    constexpr vector<T, 3> from_theta_phi_towards_z(const T& sin_theta, const T& cos_theta, const T& phi)
    {
        return vector<T, 3>
        {
            std::clamp(sin_theta, -T(1), T(1)) * std::cos(phi),
            std::clamp(sin_theta, -T(1), T(1)) * std::sin(phi),
            std::clamp(cos_theta, -T(1), T(1))
        };
    }
}
