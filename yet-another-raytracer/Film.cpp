#include "Film.h"
#include <png.hpp>
#include <fstream>

template <typename T>
void setToInteger(T & output, const color_real & input)
{
	unsigned int bits = sizeof(T) * 8;
	output = static_cast<T>(input * ((1U << bits) - 1));
}

void Film::SaveAsPng(const std::wstring & filename)
{
	std::ofstream stream(filename, std::ios::binary);

	if (!stream.is_open())
		throw std::exception();

	png::image<png::rgb_pixel> image(width_, height_);
	image.set_file_gamma(1 / 2.2);

	const color_rgbx color_0(0.0f, 0.0f, 0.0f, 0.0f);
	const color_rgbx color_1(1.0f, 1.0f, 1.0f, 1.0f);

	for (unsigned int y = 0; y < height_; y++)
	{
		for (unsigned int x = 0; x < width_; x++)
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
