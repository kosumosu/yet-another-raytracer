#include "SphereObject.h"

#include <exception>

SphereObject::SphereObject(void)
	: m_center(vector3(0.0, 0.0, 0.0))
	, m_radius(1.0)
{
}


SphereObject::~SphereObject(void)
{
}

Hit SphereObject::FindHit( const Ray & ray ) const
{
	auto inversed_ray = ray.Transform(this->inverse_transform());

	// from center of sphere to ray origin
	auto center_to_origin = inversed_ray.origin() - m_center;

	space_real a = math::length2(inversed_ray.direction());
	space_real b = space_real(2.0) * math::dot(inversed_ray.direction(), center_to_origin);
	space_real c = math::length2(center_to_origin) - m_radius * m_radius;

	space_real d = b * b - space_real(4.0) * a * c;

	if (d < space_real(0.0))
	{
		return Hit();
	}
	else // treat d == 0 as d > 0 i.e. think that we have two roots
	{
		space_real d_rooted = std::sqrt(d);
		space_real x1 = (b + d_rooted) / (space_real(-2.0) * a);
		space_real x2 = (b - d_rooted) / (space_real(-2.0) * a);

		if ( x1 <= space_real(0.0) && x2 <= space_real(0.0))
		{
			return Hit();
		}
		else
		{
			space_real x;
			if (x1 * x2 > space_real(0.0)) // if outside of the sphere (signs are the same)
			{
				x = std::min(x1, x2);
			}
			else
			{
				x = std::max(x1, x2);
			}

			auto hit_point = inversed_ray.origin() + inversed_ray.direction() * x;
			auto normal = hit_point - m_center; // normal is non-normalized

			auto transformed_hit_point = (this->transform() * vector4(hit_point, 1.0)).reduce();
			auto transformed_normal = math::normalize((this->normal_transform() * vector4(normal, space_real(0.0))).reduce());

			return Hit(transformed_hit_point, transformed_normal, this, x);
		}
	}
}

bool SphereObject::DoesHit( const Ray & ray ) const
{
	auto inversed_ray = ray.Transform(this->inverse_transform());

	auto center_to_origin = inversed_ray.origin() - m_center;

	space_real a = math::length2(inversed_ray.direction());
	space_real b = space_real(2.0) * math::dot(inversed_ray.direction(), center_to_origin);
	space_real c = math::length2(center_to_origin) - m_radius * m_radius;

	space_real d = b * b - space_real(4.0) * a * c;

	if (d < space_real(0.0))
		return false;

	space_real d_rooted = std::sqrt(d);
	space_real x1 = (b + d_rooted) / (space_real(-2.0) * a);
	space_real x2 = (b - d_rooted) / (space_real(-2.0) * a);

	return x1 > space_real(0.0) || x2 > space_real(0.0);
}

void SphereObject::PrepareForRendering()
{
	auto scale = extractBoundsScale(transform()).reduce();
	auto translate = extractTranslate(transform());

	BoundingBox bbox;
	bbox.min_corner((m_center + vector3(-m_radius)) * scale + translate);
	bbox.max_corner((m_center + vector3(m_radius)) * scale + translate);

	bbox.ExtendForEpsilon();

	bounding_box(bbox);
}
