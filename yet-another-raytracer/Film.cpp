#include "Film.h"

#include <png.hpp>
#include <fstream>
#include <limits>

#define TRC_GAMMA_22 1
#define TRC_SRGB 2

#define TRC_MODE TRC_GAMMA_22


namespace
{
    color_real linear_to_srgb(color_real val)
    {
        return val < 0.0031308f
                   ? val * 12.92f
                   : 1.055f * powf(val, 1.0f / 2.4f) - 0.055f;
    }

#if TRC_MODE == TRC_SRGB
    color_real linear_to_storage(color_real val)
    {
        return linear_to_srgb(val);
    }

    color_rgb linear_to_storage(const color_rgb& val)
    {
        return color_rgb{
            linear_to_storage(val[0]),
            linear_to_storage(val[1]),
            linear_to_storage(val[2])
        };
    }
#elif TRC_MODE == TRC_GAMMA_22

    color_rgb linear_to_storage(const color_rgb& val)
    {
        return math::pow(val, color_real(1.0f / 2.2f));
    }
#else
#error "Unexpected TRC_MODE"
#endif
}

template <typename T>
void setToInteger(T& output, const color_real& input)
{
    output = T(input * std::numeric_limits<T>::max());
}

void Film::SaveAsPng(const std::filesystem::path& filename)
{
    std::ofstream stream(filename, std::ios::binary);

    if (!stream.is_open())
        throw std::exception();

    png::image<png::rgb_pixel> image(width(), height());
    //image.set_gamma(1 / 2.2);

    constexpr color_rgb color_0(0.0f, 0.0f, 0.0f);
    constexpr color_rgb color_1(1.0f, 1.0f, 1.0f);

    for (unsigned int y = 0; y < height(); y++)
    {
        for (unsigned int x = 0; x < width(); x++)
        {
            const auto gammaCorrected = linear_to_storage(getPixel(x, y).reduce());
            const auto clamped = math::clamp(gammaCorrected, color_0, color_1);

            setToInteger(image[y][x].red, clamped[0]);
            setToInteger(image[y][x].green, clamped[1]);
            setToInteger(image[y][x].blue, clamped[2]);
        }
    }

    image.write_stream(stream);
}
