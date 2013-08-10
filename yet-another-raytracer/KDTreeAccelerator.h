#pragma once

#include "Accelerator.h"
#include "KDTreeNode.h"

class __declspec(align(MM_ALIGNMENT)) KDTreeAccelerator : public Accelerator
{
public:
	KDTreeAccelerator(const ObjectCollection * objects);
	virtual ~KDTreeAccelerator(void);

	virtual Marcher * StartMarching( const Ray & ray, space_real near, space_real far ) const;

private:
	KDTreeNode m_root_node;
	BoundingBox m_scene_box;
	unsigned int m_max_depth;
};

