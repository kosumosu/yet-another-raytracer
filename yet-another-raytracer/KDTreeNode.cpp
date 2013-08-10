#include "KDTreeNode.h"

#include <algorithm>

#define OPTIMAL_PRIMITIVES_PER_LEAF 4

KDTreeNode::KDTreeNode(void)
{
}


KDTreeNode::~KDTreeNode(void)
{
	if (m_leaf)
		delete m_objects;
}

unsigned int KDTreeNode::Build(const BoundingBox & box, ObjectCollection * objects)
{
	_ASSERT(!box.empty());
	//_ASSERT(objects->size() != 0);

	if (objects->size() <= OPTIMAL_PRIMITIVES_PER_LEAF)
	{
		return BeacomeALeaf(objects);
	}
	else
	{
		return BecomeABranch(box, objects);
	}
}

unsigned int KDTreeNode::BeacomeALeaf( const ObjectCollection * objects )
{
	m_leaf = true;
	m_objects = objects;

	return 1;
}

unsigned int KDTreeNode::BecomeABranch( const BoundingBox & box, ObjectCollection * objects )
{
	m_split_axis = GetSplittingAxis(box);
	SortObjects(objects);

	m_split_coord = FindSplittingPlane(box, objects);
	unsigned int depth = GenerateSubNodes(box, objects);

	return depth;
}

unsigned int KDTreeNode::GetSplittingAxis( const BoundingBox & box ) const
{
	auto diff = box.max_corner() - box.min_corner();
	
	unsigned int max_axis = 0;
	for (unsigned int i = 1; i < diff.size(); i++)
	{
		if (diff[i] > diff[max_axis])
			max_axis = i;
	}

	return max_axis;
}


void KDTreeNode::SortObjects( ObjectCollection * objects )
{
	std::sort(objects->begin(), objects->end(),
		[&] (ObjectCollection::const_reference obj0, ObjectCollection::const_reference obj1)
	{
		return obj0->bounding_box().min_corner()[m_split_axis] < obj1->bounding_box().min_corner()[m_split_axis];
	});
}


space_real KDTreeNode::FindSplittingPlane( const BoundingBox & box, const ObjectCollection * sorted_objects ) const
{
	return (box.min_corner()[m_split_axis] + box.max_corner()[m_split_axis]) * (space_real)0.5;

	//auto split_index = sorted_objects->size() / 2;
	//return (*sorted_objects)[split_index]->bounding_box().min_corner()[m_split_axis];
}

unsigned int KDTreeNode::GenerateSubNodes( const BoundingBox & box, const ObjectCollection * objects )
{
	BoundingBox left_box, right_box;
	box.Split(m_split_axis, m_split_coord, left_box, right_box);

	ObjectCollection * left_objects = new ObjectCollection();
	ObjectCollection * right_objects = new ObjectCollection();
	left_objects->reserve(objects->size());
	right_objects->reserve(objects->size());

	for (auto & object : *objects)
	{
		if (object->bounding_box().Intersects(left_box))
			left_objects->push_back(object);
		if (object->bounding_box().Intersects(right_box))
			right_objects->push_back(object);
	}

	if ((left_objects->size() + right_objects->size() > objects->size() * 3 / 2) /*|| left_objects->empty() || right_objects->empty()*/) // doesn't worth to split
	{
		delete left_objects;
		delete right_objects;
		return BeacomeALeaf(objects);
	}
	else
	{
		delete objects;

		m_leaf = false;
		m_subnodes[0] = new KDTreeNode();
		m_subnodes[1] = new KDTreeNode();

		unsigned int left_depth = m_subnodes[0]->Build(left_box, left_objects);
		unsigned int right_depth = m_subnodes[1]->Build(right_box, right_objects);

		return std::max(left_depth, right_depth) + 1;
	}
}

