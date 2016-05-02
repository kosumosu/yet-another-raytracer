#pragma once

#include "Accelerator.h"
#include <vector>

class NullAccelerator : public Accelerator
{
public:
	NullAccelerator(const ObjectCollection & objects);
	~NullAccelerator(void);

	virtual Marcher * CreateMarcher() const override;

private:
	std::vector<GeometryObject*> m_objects;
};

