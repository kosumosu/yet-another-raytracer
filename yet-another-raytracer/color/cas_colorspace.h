#pragma once

#include <Types.h>
#include <math/base_matrix.hpp>
#include <math/base_matrix_functions.hpp>


namespace color
{
    // Colorspace.
    // Following http://www.thetenthplanet.de/archives/4519, we adopt
    // a chromatic adapation space (which we will unimaginatively call
    // CAS) with the aim to make the scattering computations *independent* per
    // channel, see the post for details.
    // The channels in this space ~correspond to point sampling
    // the spectrum at 615 nm, 535 nm, and 445 nm, respectively.

    constexpr auto CAS2RGB = math::base_matrix<color_real, 3, 3>(
        color_rgb{1.6218, -0.4493, 0.0325},
        color_rgb{-0.0374, 1.0598, -0.0742},
        color_rgb{-0.0283, -0.1119, 1.0491});

    constexpr auto RGB2CAS = math::base_matrix<color_real, 3, 3>(
        color_rgb{6.2267e-1, 2.6392e-1, -6.2375e-4},
        color_rgb{2.3324e-2, 9.6056e-1, 6.7215e-2},
        color_rgb{1.9285e-2, 1.0958e-1, 9.6035e-1});

    constexpr color_rgb rgb_to_cas(const color_rgb& color)
    {
        return RGB2CAS * color;
    }

    constexpr color_rgb cas_to_rgb(const color_rgb& color)
    {
        return CAS2RGB * color;
    }
}
