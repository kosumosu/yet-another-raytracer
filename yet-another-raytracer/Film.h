#pragma once

#include "Types.h"

class Film
{
public:
	Film(unsigned int width, unsigned int height)
		: m_width(width)
		, m_height(height)
		, m_pixels(new color_rgbx[width * height])
	{

	}

	~Film(void)
	{
		delete[] m_pixels;
	}

	unsigned int width() const { return m_width; }
	void width(unsigned int val) { m_width = val; }

	unsigned int height() const { return m_height; }
	void height(unsigned int val) { m_height = val; }

	// Returns pointer to pixel buffer
	color_rgbx * pixels() const { return m_pixels; }

	// Returns pointer to the beginning of the line_num-th line.
	color_rgbx * line_at(unsigned int line_num) const { return m_pixels + m_width * line_num; }

	// Returns pointer to the (x, y) pixel.
	color_rgbx * pixel_at(unsigned int x, unsigned int y) const { return line_at(y) + x; }


	void SaveAsPng(const std::wstring & filename);

private:
	unsigned int m_width;
	unsigned int m_height;

	color_rgbx * m_pixels;

};

