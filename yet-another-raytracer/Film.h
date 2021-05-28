#pragma once

#include "Types.h"

class Film
{
public:
	Film(uint_vector2 size)
		: size_{ std::move(size) }
		, pixels_{ std::size_t(size_[0]) * size_[1], color_rgbx::zero() }
	{
	}

	[[nodiscard]] unsigned int width() const { return size_[0]; }
	[[nodiscard]] unsigned int height() const { return size_[1]; }

	[[nodiscard]] const std::vector<color_rgbx>& pixels() const { return pixels_; }

	[[nodiscard]] const color_rgbx& getPixel(unsigned int x, unsigned int y) const { return pixels_[std::size_t(y) * width() + x]; }
	void setPixel(uint_vector2 coord, const color_rgbx& value) { pixels_[std::size_t(coord[1]) * width() + coord[0]] = value; }

	void transferFilm(const Film& sourceFilm, const uint_vector2& destinationMinCoord, const uint_vector2& blockSize)
	{
		for (unsigned int y = 0; y < sourceFilm.height(); ++y)
		{
			const auto destY = y + destinationMinCoord[1];

			for (unsigned int x = 0; x < sourceFilm.width(); ++x)
			{
				setPixel({ x + destinationMinCoord[0], destY }, sourceFilm.getPixel(x, y));
			}
		}
	}

	void SaveAsPng(const std::wstring& filename);

	[[nodiscard]] uint_vector2 size() const
	{
		return size_;
	}

private:
	uint_vector2 size_;

	std::vector<color_rgbx> pixels_;
};
