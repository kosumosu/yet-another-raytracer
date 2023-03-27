#include "BitmapTexture.h"
#include <png.hpp>

using pixel_t = png::rgb_pixel_16;
using pixel_component_t = png::pixel_traits<pixel_t>::component_type;

constexpr pixel_component_t maxPixelValue = std::numeric_limits<png::pixel_traits<pixel_t>::component_type>::max();
constexpr color_real pixelValueNormalizationFactor = color_real(1.0) / color_real(maxPixelValue);
constexpr color_real gammaPower = color_real(2.2);

color_real convertChannel(const pixel_component_t& component)
{
	return std::pow(color_real(component) * pixelValueNormalizationFactor, gammaPower);
}

color_rgbx convertColor(const pixel_t& pixel)
{
	return color_rgbx(convertChannel(pixel.red), convertChannel(pixel.green), convertChannel(pixel.blue), 0);
}

BitmapTexture::BitmapTexture(const std::experimental::filesystem::path& filename)
	: size_{0, 0}
	, halfTexel_{vector2::zero()}
{
	std::ifstream stream(filename, std::ifstream::binary);
	const png::image<pixel_t> image(stream);

	size_ = {image.get_width(), image.get_height()};
	halfTexel_ = space_real(1.0) / (size_ * space_real(2));

	texels_.resize(std::size_t(size_[0]) * size_[1], color_rgbx::zero());

	for (unsigned int y = 0; y < size_[1]; ++y)
	{
		for (unsigned int x = 0; x < size_[0]; ++x)
		{
			const auto pixel = image.get_pixel(x, y);
			texels_[std::size_t(y) * size_[0] + x] = convertColor(pixel);
		}
	}
}

color_rgbx BitmapTexture::Sample(const TextureCoords& coords) const
{
	// tiled coords
	const auto floatXY = (coords.uvs[0] - halfTexel_) * size_;
	const auto minIntXY = (math::fast_floor_int<int>(floatXY) % size_ + size_) % size_;

	const auto maxIntXY = (minIntXY + int_vector2{1, 1}) % size_;

	const auto st = floatXY - math::floor(floatXY);

	const auto weight_s0t0 = color_real((space_real(1) - st[0]) * (space_real(1) - st[1]));
	const auto weight_s0t1 = color_real((space_real(1) - st[0]) * st[1]);
	const auto weight_s1t0 = color_real(st[0] * (space_real(1) - st[1]));
	const auto weight_s1t1 = color_real(st[0] * st[1]);

	const auto width = std::size_t(size_[0]);
	return texels_[minIntXY[1] * width + minIntXY[0]] * weight_s0t0 + texels_[minIntXY[1] * width + maxIntXY[0]] * weight_s1t0 + texels_[maxIntXY[1] * width +
		minIntXY[0]] * weight_s0t1 + texels_[maxIntXY[1] * width + maxIntXY[0]] * weight_s1t1;
}
