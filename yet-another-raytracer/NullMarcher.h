#pragma once

#include "Marcher.h"
#include <memory>

class NullMarcher : public Marcher
{
public:
	// for performance issues we are not using smart pointers
	NullMarcher(const ObjectCollection * objects);
	virtual ~NullMarcher(void);

	virtual const ObjectCollection * GetCurrentObjects();

	virtual bool MarcheNext();

private:
	const ObjectCollection * m_objects;
	bool m_marched;
};

