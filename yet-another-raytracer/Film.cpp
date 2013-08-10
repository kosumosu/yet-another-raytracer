#include "Film.h"
#include <png.hpp>
#include <iostream>



void Film::SaveAsPng( const char * filename )
{
	std::ofstream stream(filename, std::ios::binary);

	if(!stream.is_open())
		throw std::exception();

	png::image<png::rgb_pixel> image(m_width, m_height);

	const color4 color_1(1.0f, 1.0f, 1.0f, 1.0f);
	const color4 color_0(0.0f, 0.0f, 0.0f, 0.0f);

	for (unsigned int y = 0; y < m_height; y++)
	{
		for (unsigned int x = 0; x < m_width; x++)
		{
			auto clamped = math::clamp(*pixel_at(x, y), color_0, color_1);
			image[y][x].red = (int)(clamped.x() * 255);
			image[y][x].green = (int)(clamped.y() * 255);
			image[y][x].blue = (int)(clamped.z() * 255);
		}
	}

	image.write_stream(stream);
}
