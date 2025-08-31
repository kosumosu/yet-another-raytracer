#pragma once

#include "TextureCoords.h"

class Texture
{
public:
	virtual ~Texture() = default;

	[[nodiscard]] virtual color_rgbx Sample(const TextureCoords & coords) const = 0;
};
