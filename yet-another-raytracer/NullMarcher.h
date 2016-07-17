#pragma once

#include "Marcher.h"
#include <memory>

class NullMarcher : public Marcher
{
public:
	// for performance issues we are not using smart pointers
	NullMarcher(const std::vector<GeometryObject*> * objects);
	virtual ~NullMarcher(void);

	void Restart(const Ray & ray, space_real near, space_real far) override;

	const std::vector<GeometryObject*> * GetCurrentObjects() override;

	bool MarcheNext() override;

	bool IsCorrectIntersectionForCurrentState(space_real dist) const override;

private:
	const std::vector<GeometryObject*> * m_objects;
	bool m_marched;
};

