#include "Film.h"

#include <png.hpp>
#include <fstream>
#include <limits>


template <typename T>
void setToInteger(T& output, const color_real& input)
{
    output = T(input * std::numeric_limits<T>::max());
}

void Film::SaveAsPng(const std::filesystem::path& filename) const
{
    std::ofstream stream(filename, std::ios::binary);

    if (!stream.is_open())
        throw std::exception();

    png::image<png::rgb_pixel_16> image(width(), height());
    //image.set_gamma(1 / 2.2);

    for (unsigned int y = 0; y < height(); y++)
    {
        for (unsigned int x = 0; x < width(); x++)
        {
            const auto gammaCorrected = getPixelTonemapped(x, y);
            setToInteger(image[y][x].red, gammaCorrected[0]);
            setToInteger(image[y][x].green, gammaCorrected[1]);
            setToInteger(image[y][x].blue, gammaCorrected[2]);
        }
    }

    image.write_stream(stream);
}
