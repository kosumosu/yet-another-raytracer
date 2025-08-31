#pragma once
#include "Texture.h"

#include <filesystem>

class BitmapTexture : public Texture
{
public:
	explicit BitmapTexture(const std::filesystem::path& filename, vector2 uvShift);

	color_rgbx Sample(const TextureCoords & coords) const override;
private:
	vector2 uvShift_;
	int_vector2 size_; // keep it signed for to avoid problems with modulo operator for negative numbers. Note: (-1 % signed) != (-1 % unsigned).
	std::vector<color_rgbx> texels_;
	vector2 halfTexel_;
};
