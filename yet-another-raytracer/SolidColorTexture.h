#pragma once
#include "Texture.h"

class SolidColorTexture : public Texture
{
public:
	explicit SolidColorTexture(const color_rgbx & color)
		: _color(color) {}

	color_rgbx Sample(const TextureCoords & coords) const override
	{
		return _color;
	}

private:
	color_rgbx _color;
};
