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
	surface_point(const vector3 & point, const vector3 & normal, const uvs_t & uvs)
		: point(point),
		  normal(normal),
		  uvs(uvs) { }

	vector3 point;
	vector3 normal;
	uvs_t uvs;
};

class GeometryObject
{
public:


	GeometryObject() { }

	//GeometryObject(const matrix4 & transform, std::shared_ptr<Material> & material)
	//	//: SceneObject(transform)
	//	//: m_material(material)
	//{
	//}

	virtual ~GeometryObject(void) { }


	// Finds point of intersection with given ray
	virtual Hit FindHit(const ray3 & ray, space_real minDistance, space_real maxDistance) const = 0;

	// Determines if ray intersects the object
	virtual bool DoesHit(const ray3 & ray, space_real minDistance, space_real maxDistance) const = 0;


	// Perform some precalculations (normals, bboxes etc)
	virtual void PrepareForRendering() = 0;

	// Gets transformed bounding box
	bounding_box3 bounding_box() const { return _bounding_box; }

	// Gets bounds of that part of the object that lies within given box.
	virtual bounding_box3 GetBoundsWithinBounds(const bounding_box3 & box) const = 0;

	virtual space_real GetOneSidedSurfaceArea() const = 0;

	virtual math::random_sample<surface_point, space_real> PickRandomPointOnSurface(math::UniformRandomBitGenerator<unsigned int> & engine) const = 0;

	const Material * material() const { return _material; }
	void material(const Material * val) { _material = val; }

protected:
	void bounding_box(const bounding_box3 & val) { _bounding_box = val; }

private:
	const Material * _material;
	bounding_box3 _bounding_box;
};

typedef std::vector<std::shared_ptr<GeometryObject>> ObjectCollection;
