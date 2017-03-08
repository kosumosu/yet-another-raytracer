#pragma once

#include "TextureCoords.h"

class Texture
{
public:
	virtual ~Texture() = default;

	virtual color_rgbx Sample(const TextureCoords & coords) const = 0;
};
