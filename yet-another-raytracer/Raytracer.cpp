#include "Raytracer.h"
#include <exception>

Raytracer::Raytracer(const Accelerator * accelerator)
	: m_accelerator(accelerator)
{
	m_accelerator = accelerator;
}


Raytracer::~Raytracer(void)
{
}

Hit Raytracer::TraceRay( const Ray & ray, space_real bias ) const
{
	return TraceRay(ray, bias, std::numeric_limits<space_real>::max());
}

Hit Raytracer::TraceRay( const Ray & ray, space_real minDistance, space_real maxDistance ) const
{
	std::auto_ptr<Marcher> marcher(m_accelerator->StartMarching(ray, minDistance, maxDistance));
	while (marcher->MarcheNext())
	{
		auto objects = marcher->GetCurrentObjects();

		Hit nearest_hit;

		auto iterator = objects->begin();

		// finding first hit
		for (; iterator != objects->end(); iterator++)
		{
			auto object = iterator->get();
			auto hit = object->FindHit(ray);
			if (hit.has_occurred() && hit.distance() >= minDistance && hit.distance() <= maxDistance)
			{
				nearest_hit = hit;
				iterator++;
				break;
			}
		}

		//finding consecutive hits
		for (; iterator != objects->end(); iterator++)
		{
			auto object = iterator->get();
			auto hit = object->FindHit(ray);
			if (hit.has_occurred() && hit.distance() >= minDistance && hit.distance() <= maxDistance)
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

bool Raytracer::DoesIntersect( const Ray & ray, space_real minDistance, space_real maxDistance ) const
{
	std::auto_ptr<Marcher> marcher(m_accelerator->StartMarching(ray, minDistance, maxDistance));
	while (marcher->MarcheNext())
	{
		auto objects = marcher->GetCurrentObjects();

		for (auto iterator = objects->begin(); iterator != objects->end(); iterator++)
		{
			auto object = iterator->get();
			auto hit = object->FindHit(ray);
			if (hit.has_occurred() && hit.distance() >= minDistance && hit.distance() <= maxDistance)
			{
				return true;
			}
		}
	}

	// Nothing found
	return false;
}
