#include "Film.h"

#include <png.hpp>

#include <ImfRgbaFile.h>
#include <ImfArray.h>

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

void Film::SaveAsExr(const std::filesystem::path &filename) const {

    Imf::Array2D<Imf::Rgba> pixels(height(), width());

    for (unsigned int y = 0; y < height(); y++)
    {
        for (unsigned int x = 0; x < width(); x++)
        {
            const auto film_pixel = tonemap_(getAveragePixelValue(x, y));
            pixels[y][x] = Imf::Rgba{ film_pixel[0], film_pixel[1], film_pixel[2] };
        }
    }

    Imf::RgbaOutputFile output_file { filename.string().c_str(), static_cast<int>(width()), static_cast<int>(height()), Imf::WRITE_RGB };
    output_file.setFrameBuffer(&pixels[0][0], 1, width());
    output_file.writePixels(height());
}
