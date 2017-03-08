#pragma once

#include "Types.h"

class TextureCoords
{
public:
	TextureCoords(const vector3 & worldSpacePoint, const vector3 & worldSpaceNormal, const std::array<vector2, 1> & vectors)
		: worldSpacePoint(worldSpacePoint),
		  worldSpaceNormal(worldSpaceNormal),
		  uvs(vectors) {}

	vector3 worldSpacePoint;
	vector3 worldSpaceNormal;

	std::array<vector2, 1> uvs;
};
