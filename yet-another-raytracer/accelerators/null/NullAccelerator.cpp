#include "NullAccelerator.h"
#include "NullMarcher.h"
#include <algorithm>

namespace accelerators::null
{
	NullAccelerator::NullAccelerator(const ObjectCollection & objects)
	{
		m_objects.resize(objects.size());
		std::transform(std::begin(objects), std::end(objects), std::begin(m_objects), [](auto objectPtr) { return objectPtr.get(); });
	}

	NullMarcher NullAccelerator::CreateMarcher() const
	{
		return NullMarcher(&m_objects);
	}
}