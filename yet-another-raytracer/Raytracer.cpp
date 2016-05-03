#include "Raytracer.h"
#include <exception>


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
	auto biasedRay = ray.MoveOriginAlongDirection(minDistance);
	m_marcher->Restart(biasedRay, 0, maxDistance);
	while (m_marcher->MarcheNext())
	{
		auto objects = m_marcher->GetCurrentObjects();

		Hit nearest_hit;

		auto iterator = objects->begin();

		// finding first hit
		for (; iterator != objects->end(); iterator++)
		{
			auto object = *iterator;
			auto hit = object->FindHit(biasedRay);
			if (hit.has_occurred() && hit.distance() >= minDistance && hit.distance() <= maxDistance && m_marcher->IsDistanceWithinCurrentBounds(hit.distance()))
			{
				nearest_hit = hit;
				iterator++;
				break;
			}
		}

		//finding consecutive hits
		for (; iterator != objects->end(); iterator++)
		{
			auto object = *iterator;
			auto hit = object->FindHit(biasedRay);
			if (hit.has_occurred() && hit.distance() >= minDistance && hit.distance() <= maxDistance && m_marcher->IsDistanceWithinCurrentBounds(hit.distance()))
			{
				if (hit.distance() < nearest_hit.distance())
				{
					nearest_hit = hit;
				}
			}
		}

		// If found
		if (nearest_hit.has_occurred())
			return nearest_hit;
	}

	// Nothing found
	return Hit();
}

bool Raytracer::DoesIntersect(const Ray & ray, space_real minDistance, space_real maxDistance) const
{
	auto biasedRay = ray.MoveOriginAlongDirection(minDistance);
	m_marcher->Restart(biasedRay, 0, maxDistance);
	while (m_marcher->MarcheNext())
	{
		auto objects = m_marcher->GetCurrentObjects();

		for (auto iterator = objects->begin(); iterator != objects->end(); iterator++)
		{
			auto object = *iterator;
			auto hit = object->FindHit(biasedRay);
			if (hit.has_occurred() && hit.distance() >= minDistance && hit.distance() <= maxDistance)
			{
				return true;
			}
		}
	}

	// Nothing found
	return false;
}
