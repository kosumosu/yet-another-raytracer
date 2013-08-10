#pragma once

#include "Hit.h"
#include "Accelerator.h"
#include <memory>

class Raytracer
{
public:
	Raytracer(const Accelerator * accelerator);
	~Raytracer(void);

	Hit TraceRay(const Ray & ray, space_real bias) const;
	Hit TraceRay(const Ray & ray, space_real minDistance, space_real maxDistance) const;
	bool DoesIntersect(const Ray & ray, space_real minDistance, space_real maxDistance) const;

private:
	const Accelerator * m_accelerator;
};

