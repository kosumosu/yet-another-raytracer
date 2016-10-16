#pragma once

#include "MapCoords.h"

class Map
{
public:
	virtual ~Map() = default;

	virtual color_rgbx Sample(const MapCoords & coords) const = 0;
};
