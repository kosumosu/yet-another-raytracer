#pragma once

#include "Marcher.h"
#include "KDTraverseFrame.h"
#include <vector>


namespace accelerators::kd_tree
{
class KDTreeNode;

class KDTreeMarcher : public Marcher
{
public:
	KDTreeMarcher(const bounding_box3 & scene_box, const KDTreeNode * root_node, unsigned int max_depth);

	void Restart(const ray3 & ray, space_real near, space_real far) override;

	const std::vector<objects::GeometryObject*> * GetCurrentObjects() override;

	bool MarcheNext() override;

	bool IsCorrectIntersectionForCurrentState(space_real dist) const override;

private:
	const bounding_box3 m_scene_box;
	const KDTreeNode * m_root_node;
	ray3 m_ray;
	std::vector<KDTraverseFrame> m_stack;
	const KDTreeNode * m_current_leaf;
	bool m_failed_in_constructor;

	space_real m_dist_near;
	space_real m_dist_far;
};

}