#pragma once

#include "Accelerator.h"
#include "KDTreeNode.h"

class KDTreeAccelerator : public Accelerator
{
public:
	KDTreeAccelerator(const ObjectCollection & objects);
	KDTreeAccelerator(const KDTreeAccelerator & other) = delete;
	virtual ~KDTreeAccelerator(void);

	virtual Marcher * CreateMarcher() const override;

private:
	KDTreeNode m_root_node;
	BoundingBox m_scene_box;
	unsigned int m_max_depth;
};

