#include "NullAccelerator.h"
#include "NullMarcher.h"
#include <algorithm>

NullAccelerator::NullAccelerator(const ObjectCollection & objects)
{
	m_objects.resize(objects.size());
	std::transform(std::begin(objects), std::end(objects), std::begin(m_objects), [](auto objectPtr) { return objectPtr.get(); });
}

std::unique_ptr<Marcher> NullAccelerator::CreateMarcher() const
{
	return std::make_unique<NullMarcher>(&m_objects);
}

