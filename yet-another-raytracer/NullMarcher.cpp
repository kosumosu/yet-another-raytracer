#include "NullMarcher.h"

#include <limits>

NullMarcher::NullMarcher(const std::vector<GeometryObject*> * objects)
	: m_objects(objects)
	, m_marched(false)
{
}


NullMarcher::~NullMarcher(void)
{
}

void NullMarcher::Restart(const Ray & ray, space_real near, space_real)
{
	m_marched = false;
}

const std::vector<GeometryObject*> * NullMarcher::GetCurrentObjects()
{
	return m_objects;
}

bool NullMarcher::MarcheNext()
{
	if (m_marched)
	{
		return false;
	}
	else
	{
		m_marched = true;
		return true;
	}
}

bool NullMarcher::IsDistanceWithinCurrentBounds(space_real dist) const
{
	return true;
}
