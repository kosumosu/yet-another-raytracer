#pragma once

#include "Types.h"
#include <istream>

class PareserHelper
{
public:
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

	template<typename T>
	static T Read(std::istream & stream)
	{
		T x;
		stream >> x;
		return x;
	}

private:
	PareserHelper(void)
	{
	}

	~PareserHelper(void)
	{
	}
};

