#pragma once

#include "Hit.h"
#include "Types.h"
#include "math/random_sample.hpp"
#include "Sampler.h"

#include <memory>
#include <optional>
#include <utility>
#include <vector>


namespace materials
{
	class Material;
}

namespace participating_media
{
	class ParticipatingMedium;
}

namespace objects {

struct surface_point
{
	surface_point(vector3 point, vector3 normal, uvs_t uvs)
		: point(std::move(point))
		, normal(std::move(normal))
		, uvs(std::move(uvs))
	{
	}

	vector3 point;
	vector3 normal;
	uvs_t uvs;
};

class GeometryObject
{
public:

	virtual ~GeometryObject() = default;


	// Finds point of intersection with given ray
	[[nodiscard]] virtual Hit FindHit(const ray3& ray, space_real minDistance, space_real maxDistance) const = 0;

	// Determines if ray intersects the object
	[[nodiscard]] virtual bool DoesHit(const ray3& ray, space_real minDistance, space_real maxDistance) const = 0;


	// Perform some precalculations (normals, bboxes etc)
	virtual void PrepareForRendering() = 0;

	// Gets transformed bounding box
	[[nodiscard]] bounding_box3 bounding_box() const { return boundingBox_; }

	// Gets bounds of that part of the object that lies within given box.
	[[nodiscard]] virtual bounding_box3 GetBoundsWithinBounds(const bounding_box3& box) const = 0;

	[[nodiscard]] virtual space_real GetOneSidedSurfaceArea() const = 0;

	// Pick random point
	[[nodiscard]] virtual math::random_sample<surface_point, space_real> PickRandomPointOnSurface(math::Sampler<space_real>& engine) const = 0;
	// Pick random point that lits given point on itself. illuminatedPointOnSelf should probably have uv or st coords.
	[[nodiscard]] virtual std::optional<math::random_sample<surface_point, space_real>> PickRandomPointOnSurfaceForLighting(const vector3& illuminatedPointOnSelf, math::Sampler<space_real>& engine) const = 0;

	[[nodiscard]] const materials::Material* material() const { return material_; }
	void material(const materials::Material* val) { material_ = val; }

	[[nodiscard]] const participating_media::ParticipatingMedium* inner_medium() const
	{
		return inner_medium_;
	}

	void inner_medium(const participating_media::ParticipatingMedium* value)
	{
		inner_medium_ = value;
	}


protected:
	void bounding_box(const bounding_box3& val) { boundingBox_ = val; }

private:
	const materials::Material* material_ = nullptr;
	const participating_media::ParticipatingMedium* inner_medium_ = nullptr;

private:
	bounding_box3 boundingBox_;
};

}

using ObjectCollection = std::vector<std::shared_ptr<objects::GeometryObject>>;
