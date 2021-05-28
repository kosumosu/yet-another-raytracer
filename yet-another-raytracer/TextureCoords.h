#pragma once

#include <utility>

#include "Types.h"

class TextureCoords
{
public:
	TextureCoords(vector3 worldSpacePoint, vector3 worldSpaceNormal, std::array<vector2, 1> vectors)
		: worldSpacePoint(std::move(worldSpacePoint)),
		worldSpaceNormal(std::move(worldSpaceNormal)),
		uvs(std::move(vectors)) {}

	vector3 worldSpacePoint;
	vector3 worldSpaceNormal;

	std::array<vector2, 1> uvs;
};
