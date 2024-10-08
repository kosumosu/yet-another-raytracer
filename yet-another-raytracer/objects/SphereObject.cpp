#include "SphereObject.h"

#include "math/ray_functions.hpp"

namespace objects
{
	Hit SphereObject::FindHit(const ray3& ray, space_real minDistance, space_real maxDistance) const
	{
		const auto inversed_ray = math::transform3by4x4(ray, this->inverse_transform());

		// from center of sphere to ray origin
		const auto center_to_origin = inversed_ray.origin() - m_center;

		space_real a = math::length2(inversed_ray.direction());
		space_real b = space_real(2.0) * math::dot(inversed_ray.direction(), center_to_origin);
		const auto center_to_origin_len2 = math::length2(center_to_origin);
		const auto radius_squader = m_radius * m_radius;
		space_real c = center_to_origin_len2 - radius_squader;

		space_real d = b * b - space_real(4.0) * a * c;

		if (d <= space_real(0.0))
		{
			return Hit();
		}
		else // treat d == 0 as d > 0 i.e. think that we have two roots
		{
			space_real d_rooted = std::sqrt(d);
			space_real biasedX1 = (b + d_rooted) / (space_real(-2.0) * a) - minDistance; // subtract it now to simplify calculation. Later it will be compensated.
			space_real biasedX2 = (b - d_rooted) / (space_real(-2.0) * a) - minDistance;

			if (biasedX1 < space_real(0.0) && biasedX2 < space_real(0.0))
			{
				return Hit();
			}
			else
			{
				const space_real biased_x = math::is_same_sign(biasedX1, biasedX2) ? std::min(biasedX1, biasedX2) : std::max(biasedX1, biasedX2);

				const space_real x = biased_x + minDistance;

				if (x > maxDistance)
					return Hit();

				auto world_space_hit_point = inversed_ray.origin() + inversed_ray.direction() * x;
				auto normal = (world_space_hit_point - m_center) * normal_scalar_for_inversion_; // normal is non-normalized

				auto transformed_hit_point = (this->transform() * vector4(world_space_hit_point, 1.0)).reduce();
				auto transformed_normal = math::normalize((this->normal_transform() * vector4(normal, space_real(0.0))).reduce());


				return Hit(transformed_hit_point, transformed_normal, this, x, uvs_t{vector2::zero()});
			}
		}
	}

	bool SphereObject::DoesHit(const ray3& ray, space_real minDistance, space_real maxDistance) const
	{
		auto inversed_ray = math::transform3by4x4(ray, this->inverse_transform());

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

		return (x1 >= minDistance && x1 <= maxDistance) || (x2 > minDistance && x2 <= maxDistance);
	}

	bounding_box3 SphereObject::GetBoundsWithinBounds(const bounding_box3& box) const
	{
		auto ownBox = bounding_box();
		ownBox.Intersect(box);
		return ownBox;
	}

	void SphereObject::PrepareForRendering()
	{
		auto scale = extractBoundsScale(transform()).reduce();
		auto translate = extractTranslate(transform());

		bounding_box3 bbox;
		bbox.min_corner((m_center + vector3::fill(-m_radius)) * scale + translate);
		bbox.max_corner((m_center + vector3::fill(m_radius)) * scale + translate);

		bbox.ExtendForEpsilon();

		bounding_box(bbox);
	}

	space_real SphereObject::GetOneSidedSurfaceArea() const
	{
		return space_real(4.0 * math::pi) * m_radius * m_radius;
	}

	math::random_sample<surface_point, space_real> SphereObject::PickRandomPointOnSurface(math::Sampler<space_real>& sampler) const
	{
		const auto directionFromCenter = math::sphericalRand<space_real>(sampler);
		// scale is not supported
		const auto worldPoint = transform() * vector4(m_center + directionFromCenter * m_radius, space_real(1.0));
		const auto worldNormal = normal_transform() * vector4(directionFromCenter, space_real(0.0));
		return math::random_sample<surface_point, space_real>(
			surface_point(worldPoint.reduce(), worldNormal.reduce(), uvs_t{vector2::zero()}),
			space_real(1.0) / GetOneSidedSurfaceArea(),
			false);
	}

	std::optional<math::random_sample<surface_point, space_real>> SphereObject::PickRandomPointOnSurfaceForLighting(
		const vector3& illuminatedPointOnSelf,
		math::Sampler<space_real>& sampler) const
	{
		// TODO: needs proper implementation to avoid points that are too close to each other
		return PickRandomPointOnSurface(sampler);
	}
}