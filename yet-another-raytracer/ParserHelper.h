#pragma once

#include "Types.h"
#include <istream>

class ParserHelper
{
public:
	static vector2 ReadVec2(std::istream & stream)
	{
		space_real x, y;
		stream >> x >> y;
		return vector2(x, y);
	}

	static vector3 ReadVec3(std::istream & stream)
	{
		space_real x, y, z;
		stream >> x >> y >> z;
		return vector3(x, y, z);
	}

	static space_real ReadSpaceReal(std::istream & stream)
	{
		return Read<space_real>(stream);
	}

	static color_rgb ReadColorRgb(std::istream & stream)
	{
		color_real r, g, b;
		stream >> r >> g >> b;
		return color_rgb(r, g, b);
	}

	static color_real ReadColorReal(std::istream & stream)
	{
		return Read<color_real>(stream);
	}

	static int ReadInt(std::istream & stream)
	{
		return Read<int>(stream);
	}

	static unsigned int ReadUInt(std::istream & stream)
	{
		return Read<unsigned int>(stream);
	}

	static std::string ReadId(std::istream & stream)
	{
		return Read<std::string>(stream);
	}

	template <typename T>
	static T Read(std::istream & stream)
	{
		T x;
		stream >> x;
		return x;
	}

private:
	ParserHelper(void) { }

	~ParserHelper(void) { }
};
