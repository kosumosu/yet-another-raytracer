#include "Film.h"

#include <png.hpp>
#include <fstream>
#include <limits>

template <typename T>
void setToInteger(T& output, const color_real& input)
{
	output = T(input * std::numeric_limits<T>::max());
}

void Film::SaveAsPng(const std::wstring& filename)
{
	std::ofstream stream(filename, std::ios::binary);

	if (!stream.is_open())
		throw std::exception();

	png::image<png::rgb_pixel> image(width(), height());
	image.set_file_gamma(1 / 2.2);

	constexpr color_rgbx color_0(0.0f, 0.0f, 0.0f, 0.0f);
	constexpr color_rgbx color_1(1.0f, 1.0f, 1.0f, 1.0f);

	for (unsigned int y = 0; y < height(); y++)
	{
		for (unsigned int x = 0; x < width(); x++)
		{
			const auto gammaCorrected = math::pow(getPixel(x, y), color_real(1 / 2.2));
			const auto clamped = math::clamp(gammaCorrected, color_0, color_1);

			setToInteger(image[y][x].red, clamped[0]);
			setToInteger(image[y][x].green, clamped[1]);
			setToInteger(image[y][x].blue, clamped[2]);
		}
	}

	image.write_stream(stream);
}
