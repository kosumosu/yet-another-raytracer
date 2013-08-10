#include "NullMarcher.h"

#include <limits>

NullMarcher::NullMarcher(const ObjectCollection * objects)
	: m_objects(objects)
	, m_marched(false)
{
}


NullMarcher::~NullMarcher(void)
{
}

const ObjectCollection * NullMarcher::GetCurrentObjects()
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
