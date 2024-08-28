#pragma once

#include "Types.h"

namespace color
{
	constexpr auto LUMINOCITY_WEIGHTS = color_rgb{0.2126, 0.7152, 0.0722 };

	constexpr color_real get_importance(const color_rgb & color)
	{
		// simple averaging for now
		//return (color[0] + color[1] + color[2]) / color_real(3.0);
		//return color[0] * color_real(0.2126) + color[1] * color_real(0.7152) + color[2] * color_real(0.0722); // BT. 709
		return math::dot(color, LUMINOCITY_WEIGHTS);
	}
}