#pragma once

#include "Hit.h"
#include "Types.h"
#include "random_sample.hpp"
#include "UniformRandomBitGenerator.h"

#include <memory>
#include <vector>
#include <functional>


class Material;

struct surface_point
{
	surface_point(const vector3 & point, const vector3 & normal): point(point), normal(normal) {
	}

	vector3 point;
	vector3 normal;
};

class GeometryObject
{
public:


	GeometryObject(void)
	{
	}

	//GeometryObject(const matrix4 & transform, std::shared_ptr<Material> & material)
	//	//: SceneObject(transform)
	//	//: m_material(material)
	//{
	//}

	virtual ~GeometryObject(void)
	{
	}


	// Finds point of intersection with given ray
	virtual Hit FindHit(const ray3 & ray, space_real minDistance, space_real maxDistance) const = 0;

	// Determines if ray intersects the object
	virtual bool DoesHit(const ray3 & ray, space_real minDistance, space_real maxDistance) const = 0;


	// Perform some precalculations (normals, bboxes etc)
	virtual void PrepareForRendering() = 0;

	// Gets transformed bounding box
	bounding_box3 bounding_box() const { return m_bounding_box; }

	// Gets bounds of that part of the object that lies within given box.
	virtual bounding_box3 GetBoundsWithinBounds(const bounding_box3 & box) const = 0;

	virtual space_real GetOneSidedSurfaceArea() const = 0;

	virtual math::random_sample<surface_point, space_real> PickRandomPointOnSurface(math::UniformRandomBitGenerator<unsigned int> & engine) const = 0;

	const std::shared_ptr<Material> & material() const { return m_material; }
	void material(const std::shared_ptr<Material> & val) { m_material = val; }

protected:
	void bounding_box(const bounding_box3 & val) { m_bounding_box = val; }

private:
	std::shared_ptr<Material> m_material;
	bounding_box3 m_bounding_box;
};

typedef std::vector< std::shared_ptr<GeometryObject> > ObjectCollection;