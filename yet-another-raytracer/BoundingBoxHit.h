#pragma once

#include "Types.h"

class BoundingBoxHit
{
public:

	BoundingBoxHit()
		: m_occurred(false)
	{

	}

	BoundingBoxHit(space_real near, space_real far)
		: m_near(near)
		, m_far(far)
		, m_occurred(true)
	{

	}

	BoundingBoxHit(space_real near, space_real far, bool occured)
		: m_near(near)
		, m_far(far)
		, m_occurred(occured)
	{

	}

	~BoundingBoxHit(void) { }

	space_real near() const { return m_near; }

	space_real far() const { return m_far; }

	bool occurred() const { return m_occurred; }

	bool is_inside_box() const { return m_near < (space_real)0.0 && m_far >= (space_real)0.0; }

private:
	space_real m_near;
	space_real m_far;
	bool m_occurred;
};

