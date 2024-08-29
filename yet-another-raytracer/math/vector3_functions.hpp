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
}
