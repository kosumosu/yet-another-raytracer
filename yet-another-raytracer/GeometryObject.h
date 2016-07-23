#pragma once

#include "Hit.h"
#include "BoundingBox.hpp"
#include "Types.h"
#include "random_sample.hpp"
#include "UniformRandomBitGenerator.h"

#include <memory>
#include <vector>
#include <functional>


class Material;

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
	virtual Hit FindHit(const Ray & ray, space_real minDistance, space_real maxDistance) const = 0;

	// Determines if ray intersects the object
	virtual bool DoesHit(const Ray & ray, space_real minDistance, space_real maxDistance) const = 0;


	// Perform some precalculations (normals, bboxes etc)
	virtual void PrepareForRendering() = 0;

	// Gets transformed bounding box
	BoundingBox bounding_box() const { return m_bounding_box; }

	// Gets bounds of that part of the object that lies within given box.
	virtual BoundingBox GetBoundsWithinBounds(const BoundingBox & box) const = 0;

	virtual space_real GetOneSidedSurfaceArea() const = 0;

	virtual math::random_sample<vector3, space_real> PickRandomPointOnSurface(math::UniformRandomBitGenerator<unsigned int> & engine) const = 0;

	const std::shared_ptr<Material> & material() const { return m_material; }
	void material(const std::shared_ptr<Material> & val) { m_material = val; }

protected:
	void bounding_box(const BoundingBox & val) { m_bounding_box = val; }

private:
	std::shared_ptr<Material> m_material;
	BoundingBox m_bounding_box;
};

typedef std::vector< std::shared_ptr<GeometryObject> > ObjectCollection;