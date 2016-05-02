#include "NullAccelerator.h"
#include "NullMarcher.h"
#include <algorithm>

NullAccelerator::NullAccelerator(const ObjectCollection & objects)
{
	m_objects.resize(objects.size());
	std::transform(std::begin(objects), std::end(objects), std::begin(m_objects), [](auto objectPtr) { return objectPtr.get(); });
}


NullAccelerator::~NullAccelerator(void)
{
}

Marcher * NullAccelerator::CreateMarcher() const
{
	return new NullMarcher(&m_objects);
}

