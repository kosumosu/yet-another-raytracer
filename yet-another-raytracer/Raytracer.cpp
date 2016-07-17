#include "Raytracer.h"


Raytracer::Raytracer(std::unique_ptr<Marcher> && marcher)
	: m_marcher(std::move(marcher))
{
}

Raytracer::~Raytracer(void)
{
}

Hit Raytracer::TraceRay(const Ray & ray, space_real bias) const
{
	return TraceRay(ray, bias, std::numeric_limits<space_real>::max());
}

Hit Raytracer::TraceRay(const Ray & ray, space_real minDistance, space_real maxDistance) const
{
	m_marcher->Restart(ray, minDistance, maxDistance);

	Hit nearest_hit;

	while (m_marcher->MarcheNext())
	{
		auto objects = m_marcher->GetCurrentObjects();

		for (const auto & object : *objects)
		{
			auto hit = object->FindHit(ray, minDistance, maxDistance);
			if (hit.has_occurred())
			{
				if (!nearest_hit.has_occurred() || hit.distance() < nearest_hit.distance())
				{
					nearest_hit = hit;
				}
			}
		}

		// If found and it's guaranteed to be the closest one
		if (nearest_hit.has_occurred() && m_marcher->IsCorrectIntersectionForCurrentState(nearest_hit.distance()))
			return nearest_hit;
	}

	return nearest_hit;
}

bool Raytracer::DoesIntersect(const Ray & ray, space_real minDistance, space_real maxDistance) const
{
	m_marcher->Restart(ray, minDistance, maxDistance);
	while (m_marcher->MarcheNext())
	{
		auto objects = m_marcher->GetCurrentObjects();

		for (const auto & object : *objects)
		{
			auto hit = object->FindHit(ray, minDistance, maxDistance);
			if (hit.has_occurred())
			{
				return true;
			}
		}
	}

	// Nothing found
	return false;
}
