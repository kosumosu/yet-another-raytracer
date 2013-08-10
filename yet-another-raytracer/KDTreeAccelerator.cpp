#include "KDTreeAccelerator.h"

#include "KDTreeMarcher.h"

KDTreeAccelerator::KDTreeAccelerator( const ObjectCollection * objects )
{
	BoundingBox scene_box;
	ObjectCollection * objects_copy = new ObjectCollection(objects->begin(), objects->end());

	for (auto & object : *objects)
	{
		scene_box.Include(object->bounding_box());
	}

	m_scene_box = scene_box;

	m_max_depth = m_root_node.Build(scene_box, objects_copy);
}


KDTreeAccelerator::~KDTreeAccelerator(void)
{
}

Marcher * KDTreeAccelerator::StartMarching( const Ray & ray, space_real near, space_real far ) const
{
	return new KDTreeMarcher(ray, m_scene_box, &m_root_node, near, far, m_max_depth);
}
