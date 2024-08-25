#pragma once

#include "../Accelerator.h"
#include "KDTreeNode.h"
#include "KDTreeMarcher.h"

namespace accelerators::kd_tree
{

class KDTreeAccelerator final: public Accelerator<KDTreeMarcher>
{
public:
	KDTreeAccelerator(const ObjectCollection & objects);
	KDTreeAccelerator(const KDTreeAccelerator & other) = delete;
	~KDTreeAccelerator() override = default;

	KDTreeMarcher CreateMarcher() const override;

private:
	KDTreeNode m_root_node;
	bounding_box3 m_scene_box;
	unsigned int m_max_depth;
};

}