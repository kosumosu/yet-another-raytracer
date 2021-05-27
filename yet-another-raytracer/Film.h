#pragma once

#include "Types.h"

class Film
{
public:
	Film(unsigned int width, unsigned int height)
		: width_(width)
		, height_(height)
		, pixels_(width * height, color_rgbx::zero())
	{
	}

	[[nodiscard]] unsigned int width() const { return width_; }
	void width(unsigned int val) { width_ = val; }

	[[nodiscard]] unsigned int height() const { return height_; }
	void height(unsigned int val) { height_ = val; }

	[[nodiscard]] const std::vector<color_rgbx>& pixels() const { return pixels_; }

	[[nodiscard]] const color_rgbx& getPixel(unsigned int x, unsigned int y) const { return pixels_[y * width_ + x]; }
	void setPixel(unsigned int x, unsigned int y, const color_rgbx& value) { pixels_[y * width_ + x] = value; }

	void transferFilm(uint_vector2 minCoord, const Film& film)
	{
		for (unsigned int y = minCoord[1]; y < minCoord[1] + film.height(); ++y)
		{
			for (unsigned int x = minCoord[0]; x < minCoord[0] + film.width(); ++x)
			{
				pixels_[y * width_ + x] = film.getPixel(x, y);
			}
		}
	}

	void SaveAsPng(const std::wstring& filename);

private:
	unsigned int width_;
	unsigned int height_;

	std::vector<color_rgbx> pixels_;
};
