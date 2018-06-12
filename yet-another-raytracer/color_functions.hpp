#pragma once

#include "Types.h"

namespace color
{
	color_real inline get_importance(const color_rgbx & color)
	{
		// simple averaging for now
		//return (color[0] + color[1] + color[2]) / color_real(3.0);
		return color[0] * color_real(0.2126) + color[1] * color_real(0.7152) + color[2] * color_real(0.0722); // BT. 709
	}
}