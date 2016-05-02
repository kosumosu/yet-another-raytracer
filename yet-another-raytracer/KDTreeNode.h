#pragma once

#include "GeometryObject.h"
#include "BoundingBox.hpp"

enum class NodeKind
{
	Near,
	Far
};

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

	const std::vector<GeometryObject*> * objects() const { return m_objects; }

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

	const KDTreeNode * GetSubnode(const vector3 & direction, NodeKind kind) const
	{
		bool isNear = kind == NodeKind::Near;
		bool isTowardsPositive = direction[m_split_axis] >= space_real(0.0);
		size_t nodeIndex = (isNear ^ isTowardsPositive) ? 1 : 0;
		return m_subnodes[nodeIndex];
	}

	// Returns maximum levels of subtree. Node becomes objects owner!
	unsigned int Build(const BoundingBox & box, std::vector<GeometryObject*> && objects);

private:
	bool m_leaf;
	unsigned int m_split_axis;
	union
	{
		struct
		{
			space_real m_split_coord;
			KDTreeNode * m_subnodes[2];
		};

		struct
		{
			const std::vector<GeometryObject*> * m_objects;
		};
	};
	
	


	unsigned int BeacomeALeaf(std::vector<GeometryObject*> && objects);
	unsigned int BecomeABranch(const BoundingBox & box, std::vector<GeometryObject*> && objects);
	unsigned int GetSplittingAxis(const BoundingBox & box, const std::vector<GeometryObject*> & objects) const;
	void SortObjects(std::vector<GeometryObject*> & objects);
	space_real FindSplittingPlane(const BoundingBox & box, unsigned int axis, const std::vector<GeometryObject*> & sorted_objects) const;
	unsigned int GenerateSubNodes(const BoundingBox & box, std::vector<GeometryObject*> && objects);
};