#pragma once

#include "Types.h"

namespace cloudscape
{
    constexpr auto CAS2RGB = math::transpose(math::base_matrix<space_real, 3, 3>(
        1.6218, -0.4493, 0.0325,
        -0.0374, 1.0598, -0.0742,
        -0.0283, -0.1119, 1.0491));

    constexpr auto RGB2CAS = math::transpose(math::base_matrix<space_real, 3, 3>(
        6.2267e-1, 2.6392e-1, -6.2375e-4,
        2.3324e-2, 9.6056e-1, 6.7215e-2,
        1.9285e-2, 1.0958e-1, 9.6035e-1));


    constexpr color_rgb linear_srgb_to_cas(const color_rgb& linear_srgb_color)
    {
        return RGB2CAS * linear_srgb_color;
    }
}
