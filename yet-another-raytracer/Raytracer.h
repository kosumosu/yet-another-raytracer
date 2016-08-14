#pragma once

#include "Hit.h"
#include "Marcher.h"
#include <memory>
#include <functional>

using accept_predicate = std::function<bool(const Hit & hit)>;

class Raytracer
{
public:
	Raytracer(std::unique_ptr<Marcher> && marcher);
	~Raytracer(void);

	Hit TraceRay(const ray3 & ray, space_real bias) const;
	// intersections with objectToIgonore are ignored if (ignoreBackface == true and ray hits it's backface) or (ignoreBackface == false and ray hits frontface).
	Hit TraceRay(const ray3 & ray, space_real minDistance, space_real maxDistance, const GeometryObject * objectToIgnore, const vector3 & directionToIgnore) const;
	bool DoesIntersect(const ray3 & ray, space_real minDistance, space_real maxDistance, const GeometryObject * objectToIgnore, const vector3 & directionToIgnore) const;

private:
	const std::unique_ptr<Marcher> m_marcher;
};

