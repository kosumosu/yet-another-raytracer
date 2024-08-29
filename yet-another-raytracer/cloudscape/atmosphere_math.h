#pragma once

#include "Types.h"

#include <cmath>
#include <numbers>

namespace cloudscape
{
    constexpr auto pi_cube = std::numbers::pi_v<space_real> * std::numbers::pi_v<space_real> * std::numbers::pi_v<space_real>;
    constexpr auto ior_air = space_real(1.0002926);
    constexpr auto ior_air_sqr = ior_air * ior_air;


    template <class T>
    constexpr T square(const T& value)
    {
        return value * value;
    }

    constexpr float optmass(
        space_real x1,
        space_real x2,
        space_real y,
        space_real density,
        space_real planet_radius,
        space_real height_scale)
    {
        // integrate rho*exp(-(x^2+y^2 - r)/s) dx from x=x1 to x2

        constexpr auto sqrtpi = std::sqrt(std::numbers::pi_v<space_real>);

        const auto height_scale_sqrt = std::sqrt(height_scale);
        const auto inv_height_scale_sqrt = std::sqrt(height_scale);

        return space_real(0.5) * sqrtpi * density * height_scale_sqrt
            * std::exp((planet_radius - square(y)) / height_scale)
            * (std::erf(x2 * inv_height_scale_sqrt) - std::erf(x1 * inv_height_scale_sqrt));
    }

    constexpr float optmass(
        const vector3& p1,
        const vector3& p2,
        space_real density,
        const vector3& planet_pos,
        space_real planet_radius,
        space_real height_scale)
    {
        const auto p1_to_p2 = p2 - p1;
        const auto p1_to_planet_center = planet_pos - p1;

        const auto p1_to_p2_len = math::length(p1_to_p2);
        const auto p1_to_p2_norm = p1_to_p2 / p1_to_p2_len;

        const auto projection = math::dot(p1_to_planet_center, p1_to_p2_norm);
        const auto line_to_planet_center = p1_to_planet_center - (p1_to_p2_norm * projection);

        const auto x1 = -projection;
        const auto x2 = p1_to_p2_len - projection;
        const auto y = math::length(line_to_planet_center);

        return optmass(x1, x2, y, density, planet_radius, height_scale);
    }

    constexpr space_real rayleigh_extinction(space_real wavelength)
    {
        constexpr auto sea_level_concentration = space_real(666.0);
        return space_real(8.0) * pi_cube * square(ior_air_sqr - space_real(1.0)) / (space_real(3.0) * sea_level_concentration * square(square(wavelength)));
    }


    // constexpr static space_real beta_total_rayleigh(space_real lambda, space_real pressure)
    // {
    //     const auto b = space_real(8) * space_real(math::pi) * space_real(math::pi) * space_real(math::pi) *
    //         space_real(3.601080081E-07) / (space_real(3) * space_real(2.545E+25) * pressure * lambda * lambda * lambda *
    //             lambda);
    //     return b * (space_real(6) + space_real(3) * space_real(0.0035)) / (space_real(6) - space_real(7) *
    //         space_real(0.0035));
    // }
    //
    // constexpr static space_real beta_total_mie(space_real lambda, space_real pressure, space_real atmosphere_turbidity)
    // {
    //     return space_real(0.434) * (atmosphere_turbidity * space_real(1E-17) * pressure) * space_real(math::pi) * (
    //         space_real(4) * space_real(math::pi) * space_real(math::pi)) / (lambda * lambda) * space_real(0.67);
    // }
}
