#pragma once

#include "GeometryObject.h"
#include "BoundingBox.h"

class KDTreeNode
{
public:
	KDTreeNode(void);
	~KDTreeNode(void);

	unsigned int split_axis() const { return m_split_axis; }

	space_real split_coord() const { return m_split_coord; }

	bool leaf() const { return m_leaf; }

	KDTreeNode * left_node() const { return m_subnodes[0]; }

	KDTreeNode * right_node() const { return m_subnodes[1]; }

	const ObjectCollection * objects() const { return m_objects; }

	const KDTreeNode * GetNearSubnode(const vector3 & direction) const
	{
		if (direction[m_split_axis] >= space_real(0.0))
			return m_subnodes[0];
		else
			return m_subnodes[1];
	}

	const KDTreeNode * GetFarSubnode(const vector3 & direction) const
	{
		if (direction[m_split_axis] < space_real(0.0))
			return m_subnodes[0];
		else
			return m_subnodes[1];
	}

	// Returns maximum levels of subtree. Node becomes objects owner!
	unsigned int Build(const BoundingBox & box, ObjectCollection * objects);

private:
	unsigned int m_split_axis;
	space_real m_split_coord;
	bool m_leaf;
	KDTreeNode * m_subnodes[2];
	const ObjectCollection * m_objects;

	unsigned int BeacomeALeaf(const ObjectCollection * objects);
	unsigned int BecomeABranch(const BoundingBox & box, ObjectCollection * objects);
	unsigned int GetSplittingAxis(const BoundingBox & box) const;
	void SortObjects(ObjectCollection * objects);
	space_real FindSplittingPlane(const BoundingBox & box, const ObjectCollection * sorted_objects) const;
	unsigned int GenerateSubNodes(const BoundingBox & box, const ObjectCollection * objects);
};

