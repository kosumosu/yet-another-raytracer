#include "NullAccelerator.h"
#include "NullMarcher.h"

NullAccelerator::NullAccelerator(const ObjectCollection * objects)
	: m_objects(objects)
{
}


NullAccelerator::~NullAccelerator(void)
{
}

Marcher * NullAccelerator::StartMarching( const Ray & ray, space_real near, space_real far ) const
{
	return new NullMarcher(m_objects);
}
