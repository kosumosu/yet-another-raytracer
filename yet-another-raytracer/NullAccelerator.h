#pragma once

#include "Accelerator.h"

class NullAccelerator : public Accelerator
{
public:
	NullAccelerator(const ObjectCollection * objects);
	~NullAccelerator(void);

	virtual Marcher * StartMarching( const Ray & ray, space_real near, space_real far ) const;

private:
	const ObjectCollection * m_objects;
};

