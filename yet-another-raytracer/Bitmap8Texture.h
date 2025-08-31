#pragma once
#include "Texture.h"

#include <cstdint>
#include <filesystem>

class Bitmap8Texture : public Texture
{
	using texel_t = math::vector<uint8_t, 3>;

	vector2 uvShift_;
	int_vector2 size_; // keep it signed for to avoid problems with modulo operator for negative numbers. Note: (-1 % signed) != (-1 % unsigned).
	std::vector<texel_t> texels_;
	vector2 halfTexel_;

public:
	explicit Bitmap8Texture(const std::filesystem::path& filename, vector2 uvShift);

	color_rgbx Sample(const TextureCoords & coords) const override;
private:
	color_rgbx getTexel(const int_vector2& coords) const;
};
