#include "BitmapTexture.h"
#include "png.hpp"

using pixel_t = png::rgb_pixel_16;
using pixel_component_t = png::pixel_traits<pixel_t>::component_type;

constexpr pixel_component_t maxPixelValue = std::numeric_limits<png::pixel_traits<pixel_t>::component_type>::max();
constexpr color_real pixelValueNormalizationFactor = color_real(1.0) / color_real(maxPixelValue);
constexpr color_real gammaPower = color_real(2.2);

color_real convertChannel(const pixel_component_t & component)
{
	return std::pow(component * pixelValueNormalizationFactor, gammaPower);
}

color_rgbx convertColor(const pixel_t & pixel)
{
	return color_rgbx(convertChannel(pixel.red), convertChannel(pixel.green), convertChannel(pixel.blue), 0);
}

BitmapTexture::BitmapTexture(const std::string & filename)
{
	std::ifstream stream(filename, std::ifstream::binary);
	const png::image<pixel_t> image(stream);

	_width = image.get_width();
	_height = image.get_height();
	_texels.resize(_width * _height, color_rgbx::zero());

	for (unsigned int y = 0; y < _height; ++y)
	{
		for (unsigned int x = 0; x < _width; ++x)
		{
			const auto pixel = image.get_pixel(x, y);
			_texels[y * _width + x] = convertColor(pixel);
		}
	}
}

color_rgbx BitmapTexture::Sample(const TextureCoords & coords) const
{
	// tiled coords
	const space_real finalU = coords.uvs[0][0] - std::floor(coords.uvs[0][0]);
	const space_real finalV = coords.uvs[0][1] - std::floor(coords.uvs[0][1]);

	const auto floatX = finalU * _width;
	const auto floatY = finalV * _height;

	const unsigned int x0 = static_cast<unsigned int>(floatX);
	const unsigned int y0 = static_cast<unsigned int>(floatY);

	const auto x1 = (x0 + 1) % _width;
	const auto y1 = (y0 + 1) % _height;

	const auto s = floatX - std::floor(floatX);
	const auto t = floatY - std::floor(floatY);

	const auto weight_s0t0 = color_real((space_real(1) - s) * (space_real(1) - t));
	const auto weight_s0t1 = color_real((space_real(1) - s) * t);
	const auto weight_s1t0 = color_real(s * (space_real(1) - t));
	const auto weight_s1t1 = color_real(s * t);

	return 
		_texels[y0 * _width + x0] * weight_s0t0
		+ _texels[y0 * _width + x1] * weight_s1t0
		+ _texels[y1 * _width + x0] * weight_s0t1
		+ _texels[y1 * _width + x1] * weight_s1t1
		;
}
