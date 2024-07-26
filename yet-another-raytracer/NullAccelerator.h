#pragma once

#include "Accelerator.h"
#include <vector>

class NullAccelerator final : public Accelerator
{
public:
	NullAccelerator(const ObjectCollection & objects);
	~NullAccelerator() override = default;

	std::unique_ptr<Marcher> CreateMarcher() const override;

private:
	std::vector<GeometryObject*> m_objects;
};

