#include "Film.h"
#include <png.hpp>
#include <iostream>

template <typename T>
void setToInteger(T & output, const color_real & input)
{
	int bits = sizeof(T) * 8;
	output = static_cast<T>(input * ((1 << bits) - 1));
}

void Film::SaveAsPng(const std::wstring & filename)
{
	std::ofstream stream(filename, std::ios::binary);

	if (!stream.is_open())
		throw std::exception();

	png::image<png::rgb_pixel> image(m_width, m_height);
	image.set_file_gamma(1.0);

	const color_rgbx color_0(0.0f, 0.0f, 0.0f, 0.0f);
	const color_rgbx color_1(1.0f, 1.0f, 1.0f, 1.0f);

	for (unsigned int y = 0; y < m_height; y++)
	{
		for (unsigned int x = 0; x < m_width; x++)
		{
			auto gammaCorrected = math::pow(*pixel_at(x, y), color_real(1 / 2.2));
			auto clamped = math::clamp(gammaCorrected, color_0, color_1);

			setToInteger(image[y][x].red, clamped[0]);
			setToInteger(image[y][x].green, clamped[1]);
			setToInteger(image[y][x].blue, clamped[2]);
		}
	}

	image.write_stream(stream);
}
