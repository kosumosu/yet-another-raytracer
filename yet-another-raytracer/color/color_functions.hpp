#pragma once

#include "Types.h"

namespace color
{
    constexpr auto LUMINOCITY_WEIGHTS = color_rgb{0.2126, 0.7152, 0.0722};

    constexpr color_real get_importance(const color_rgb& color)
    {
        // simple averaging for now
        //return (color[0] + color[1] + color[2]) / color_real(3.0);
        //return color[0] * color_real(0.2126) + color[1] * color_real(0.7152) + color[2] * color_real(0.0722); // BT. 709
        return math::dot(color, LUMINOCITY_WEIGHTS);
    }

    constexpr color_real linear_to_srgb(color_real val)
    {
        return val < color_real(0.0031308)
                   ? val * color_real(12.92)
                   : color_real(1.055) * std::pow(val, color_real(1.0 / 2.4)) - color_real(0.055);
    }

    constexpr color_real srgb_to_linear(color_real val)
    {
        if (val < color_real(0.04045))
            return val / color_real(12.92);
        else
            return pow((val + color_real(0.055)) / color_real(1.055), color_real(2.4));
    }

    constexpr color_rgb linear_to_srgb(const color_rgb& val)
    {
        return {
            linear_to_srgb(val[0]),
            linear_to_srgb(val[1]),
            linear_to_srgb(val[2])
        };
    }

    constexpr color_rgb srgb_to_linear(const color_rgb& val)
    {
        return {
            srgb_to_linear(val[0]),
            srgb_to_linear(val[1]),
            srgb_to_linear(val[2])
        };
    }

    constexpr color_rgb from_bgr_int(const std::uint32_t value)
    {
        return {
            color_real(value & 0xFF) / color_real(255.0),
            color_real((value >> 8) & 0xFF) / color_real(255.0),
            color_real((value >> 16) & 0xFF) / color_real(255.0)
        };
    }
}
