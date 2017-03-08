#pragma once
#include "Texture.h"

class VisualizeUVTexture : public Texture
{
public:
	color_rgbx Sample(const TextureCoords & coords) const override
	{
		return color_rgbx(coords.uvs[0][0], coords.uvs[0][1], 0.5, 0.0);
	}
};
