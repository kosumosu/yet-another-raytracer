#pragma once

#include "Hit.h"
#include "Marcher.h"
#include <memory>

class Raytracer
{
public:
	Raytracer(std::unique_ptr<Marcher> && marcher);
	~Raytracer(void);

	Hit TraceRay(const Ray & ray, space_real bias) const;
	Hit TraceRay(const Ray & ray, space_real minDistance, space_real maxDistance) const;
	bool DoesIntersect(const Ray & ray, space_real minDistance, space_real maxDistance) const;

private:
	const std::unique_ptr<Marcher> m_marcher;
};

