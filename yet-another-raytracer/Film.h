#pragma once

#include "Types.h"

class Film
{
public:
	Film(unsigned int width, unsigned int height)
		: m_width(width)
		, m_height(height)
		, m_pixels(width * height, color_rgbx::zero())
	{

	}

	unsigned int width() const { return m_width; }
	void width(unsigned int val) { m_width = val; }

	unsigned int height() const { return m_height; }
	void height(unsigned int val) { m_height = val; }

	const std::vector<color_rgbx> & pixels() const { return m_pixels; }

	const color_rgbx & getPixel(unsigned int x, unsigned int y) const { return m_pixels[y * m_width + x]; }
	void setPixel(unsigned int x, unsigned int y, const color_rgbx & value) { m_pixels[y * m_width + x] = value; }

	void SaveAsPng(const std::wstring & filename);

private:
	unsigned int m_width;
	unsigned int m_height;

	std::vector<color_rgbx> m_pixels;

};

