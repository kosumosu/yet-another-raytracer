#pragma once
#include "Texture.h"

#include <experimental/filesystem>

class BitmapTexture : public Texture
{
public:
    explicit BitmapTexture(const std::experimental::filesystem::path& filename);

	color_rgbx Sample(const TextureCoords & coords) const override;
private:
	int_vector2 size_; // keep it signed for to avoid problems with modulo operator for negative numbers. Note: (-1 % signed) != (-1 % unsigned).
	std::vector<color_rgbx> texels_;
	vector2 halfTexel_;
};
