#pragma once

#include "Types.h"

namespace accelerators::kd_tree
{
class KDTreeNode;

class KDTraverseFrame
{
public:

	KDTraverseFrame(space_real near, space_real far, const KDTreeNode * node)
		: m_near(near)
		, m_far(far)
		, m_node(node)
	{
	}

	~KDTraverseFrame(void)
	{
	}

	space_real near() const { return m_near; }
	void near(space_real val) { m_near = val; }

	space_real far() const { return m_far; }
	void far(space_real val) { m_far = val; }

	const KDTreeNode * node() const { return m_node; }
	void node(const KDTreeNode * val) { m_node = val; }

private:
	space_real m_near;
	space_real m_far;
	const KDTreeNode * m_node;
};

}