#pragma once

#include "Types.h"

class __declspec(align(MM_ALIGNMENT)) Ray
{
public:
	

	Ray(void)
	{
	}

	Ray(const vector3 & origin, const vector3 & direction)
		: m_origin(origin)
		, m_direction(direction)
		, m_inversed_direction(space_real(1.0) / direction)
	{

	}

	const vector3 & origin() const { return m_origin; }
	void origin(const vector3 & val) { m_origin = val; }

	const vector3 & direction() const { return m_direction; }
	void direction(const vector3 & val)
	{
		m_direction = val;
		m_inversed_direction = math::reciprocal(val);
	}

	const vector3 & inversed_direction() const { return m_inversed_direction; }
	void inversed_direction(const vector3 & val) { m_inversed_direction = val; }

	Ray Transform(const matrix4 & matrix) const
	{
		auto origin = (matrix * vector4(m_origin, space_real(1.0))).reduce();
		auto direction = (matrix * vector4(m_direction, space_real(0.0))).reduce();
		return Ray(origin, direction);
	}

private:
	vector3 m_origin;
	vector3 m_direction;
	vector3 m_inversed_direction;
};

