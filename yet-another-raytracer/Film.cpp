#include "Film.h"

#include <png++/png.hpp>

#include <ImfRgbaFile.h>
#include <ImfArray.h>

#include <fstream>
#include <limits>


template <typename T>
void setToInteger(T& output, const color_real& input)
{
    output = T(input * std::numeric_limits<T>::max());
}

void Film::TryLoadFromFile(std::istream& stream) {
    uint_vector2 size_from_file = uint_vector2::zero();

    stream.read(reinterpret_cast<char*>(&size_from_file), sizeof(size_from_file));

    if (size_from_file != size_) {
        throw std::runtime_error("Persisted data dimensions mismatch");
    }

    stream.read(reinterpret_cast<char *>(pixels_.data()), pixels_.size() * sizeof(pixels_[0]));

    stream.read(reinterpret_cast<char *>(sample_counts_.data()), sample_counts_.size() * sizeof(sample_counts_[0]));
}

void Film::PersistToFile(std::ostream& stream) const {
    stream.write(reinterpret_cast<const char*>(&size_), sizeof(size_));

    stream.write(reinterpret_cast<const char *>(pixels_.data()), pixels_.size() * sizeof(pixels_[0]));

    stream.write(reinterpret_cast<const char *>(sample_counts_.data()), sample_counts_.size() * sizeof(sample_counts_[0]));
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
