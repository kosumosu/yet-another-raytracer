#pragma once
#include "Map.h"

class SolidColorMap : public Map
{
public:
	explicit SolidColorMap(const color_rgbx & color)
		: _color(color) {}

	color_rgbx Sample(const MapCoords & coords) const override
	{
		return _color;
	}

private:
	color_rgbx _color;
};
