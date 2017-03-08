#pragma once
#include "Texture.h"

class BitmapTexture : public Texture
{
public:
	BitmapTexture(const std::string & filename);

	color_rgbx Sample(const TextureCoords & coords) const override;
private:
	unsigned int _width;
	unsigned int _height;
	std::vector<color_rgbx> _texels;
};
