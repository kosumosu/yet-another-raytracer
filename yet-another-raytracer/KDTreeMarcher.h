#pragma once

#include "Marcher.h"
#include "KDTraverseFrame.h"
#include <vector>

class KDTreeNode;

class KDTreeMarcher : public Marcher
{
public:
	KDTreeMarcher(const BoundingBox & scene_box, const KDTreeNode * root_node, unsigned int max_depth);
	~KDTreeMarcher(void);

	void Restart(const Ray & ray, space_real near, space_real far) override;

	const std::vector<GeometryObject*> * GetCurrentObjects() override;

	bool MarcheNext() override;

	bool IsCorrectIntersectionForCurrentState(space_real dist) const override;

private:
	const BoundingBox m_scene_box;
	const KDTreeNode * m_root_node;
	Ray m_ray;
	std::vector<KDTraverseFrame> m_stack;
	const KDTreeNode * m_current_leaf;
	bool m_failed_in_constructor;

	space_real m_dist_near;
	space_real m_dist_far;
};

