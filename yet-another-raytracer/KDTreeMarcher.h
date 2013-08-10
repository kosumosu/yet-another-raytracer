#pragma once

#include "Marcher.h"
#include "KDTreeNode.h"
#include "KDTraverseFrame.h"
#include <vector>

class __declspec(align(MM_ALIGNMENT)) KDTreeMarcher : public Marcher
{
public:
	KDTreeMarcher(const Ray & ray, const BoundingBox & scene_box, const KDTreeNode * root_node, space_real near, space_real far, unsigned int max_depth);
	~KDTreeMarcher(void);

	virtual const ObjectCollection * GetCurrentObjects();

	virtual bool MarcheNext();

private:
	Ray m_ray;
	std::vector<KDTraverseFrame> m_stack;
	const KDTreeNode * m_current_leaf;
	bool m_failed_in_constructor;
};

