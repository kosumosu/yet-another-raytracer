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

    constexpr color_rgb from_bgr_int(const std::uint32_t value)
    {
        return {
            color_real(value & 0xFF) / color_real(255.0f),
            color_real((value >> 8) & 0xFF) / color_real(255.0f),
            color_real((value >> 16) & 0xFF) / color_real(255.0f)
        };
    }
}
