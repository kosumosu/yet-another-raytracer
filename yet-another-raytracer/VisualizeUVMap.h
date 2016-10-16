#pragma once
#include "Map.h"

class VisualizeUVMap : public Map
{
public:
	color_rgbx Sample(const MapCoords & coords) const override
	{
		return color_rgbx(coords.uvs[0][0], coords.uvs[0][1], 0.5, 0.0);
	}
};
