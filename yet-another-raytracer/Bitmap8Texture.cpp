#include "Bitmap8Texture.h"

#include <utility>

#include "color/color_functions.hpp"

#include <png++/png.hpp>

using pixel_t = png::rgb_pixel;
using pixel_component_t = png::pixel_traits<pixel_t>::component_type;

namespace {
	constexpr pixel_component_t MAX_PIXEL_VALUE = std::numeric_limits<pixel_component_t>::max();
	constexpr std::size_t PIXEL_VALUE_COUNT = std::size_t(MAX_PIXEL_VALUE) + 1;

	constexpr std::array<color_real, PIXEL_VALUE_COUNT> calculate_srgb_to_linear_lut() {
		std::array<color_real, PIXEL_VALUE_COUNT> lut = {};
		for (std::size_t i = 0; i < PIXEL_VALUE_COUNT; ++i) {
			lut[i] = color::srgb_to_linear( color_real(i) / MAX_PIXEL_VALUE);
		}
		return lut;
	}

	// Make constexpr when stl has better support of constexpr cmath across compilers (a C++26 feature)
	const std::array<color_real, PIXEL_VALUE_COUNT> SRGB_TO_LINEAR_LUT = calculate_srgb_to_linear_lut();
}

Bitmap8Texture::Bitmap8Texture(const std::filesystem::path& filename, vector2 uvShift)
	: uvShift_{std::move(uvShift)}
	, size_{0, 0}
	, halfTexel_{vector2::zero()}
{
	std::ifstream stream(filename, std::ifstream::binary);
	const png::image<pixel_t> image(stream);

	size_ = {image.get_width(), image.get_height()};
	halfTexel_ = space_real(1.0) / (size_ * space_real(2));

	texels_.resize(std::size_t(size_[0]) * size_[1], texel_t::zero());

	for (unsigned int y = 0; y < size_[1]; ++y)
	{
		for (unsigned int x = 0; x < size_[0]; ++x)
		{
			const auto pixel = image.get_pixel(x, y);
			texels_[std::size_t(y) * size_[0] + x] = {
				pixel.red,
				pixel.green,
				pixel.blue,
			};
		}
	}
}

color_rgbx Bitmap8Texture::Sample(const TextureCoords& coords) const
{
	// tiled coords
	const auto transformedUVs = coords.uvs[0] + uvShift_;
	const auto floatXY = (transformedUVs - halfTexel_) * size_;
	const auto minIntXY = (math::fast_floor_int<int>(floatXY) % size_ + size_) % size_;

	const auto maxIntXY = (minIntXY + int_vector2{1, 1}) % size_;

	const auto st = floatXY - math::floor(floatXY);

	const auto weight_s0t0 = color_real((space_real(1) - st[0]) * (space_real(1) - st[1]));
	const auto weight_s0t1 = color_real((space_real(1) - st[0]) * st[1]);
	const auto weight_s1t0 = color_real(st[0] * (space_real(1) - st[1]));
	const auto weight_s1t1 = color_real(st[0] * st[1]);

	return getTexel({minIntXY[0], minIntXY[1]}) * weight_s0t0
	       + getTexel({maxIntXY[0], minIntXY[1]}) * weight_s1t0
	       + getTexel({minIntXY[0], maxIntXY[1]}) * weight_s0t1
	       + getTexel({maxIntXY[0], maxIntXY[1]}) * weight_s1t1;
}

color_rgbx Bitmap8Texture::getTexel(const int_vector2 &coords) const {
	const auto texel = texels_[coords[1] * std::size_t(size_[0]) + coords[0]];
	return color_rgbx{ SRGB_TO_LINEAR_LUT[texel[0]], SRGB_TO_LINEAR_LUT[texel[1]], SRGB_TO_LINEAR_LUT[texel[2]], 0 };
}
