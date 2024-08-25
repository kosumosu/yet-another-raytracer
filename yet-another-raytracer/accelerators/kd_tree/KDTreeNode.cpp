#include "KDTreeNode.h"

#include <algorithm>
#include <cassert>

namespace accelerators::kd_tree
{
    const unsigned int OPTIMAL_PRIMITIVES_PER_LEAF = 4;

    KDTreeNode::KDTreeNode()
        : m_leaf(false)
          , m_split_axis(0)
    {
    }


    KDTreeNode::~KDTreeNode()
    {
        if (m_leaf)
            delete m_objects;
    }

    unsigned int KDTreeNode::Build(const bounding_box3& box, std::vector<GeometryObject*>&& objects)
    {
        assert(!box.empty());
        //_ASSERT(objects->size() != 0);

        if (objects.size() <= OPTIMAL_PRIMITIVES_PER_LEAF)
        {
            return BeacomeALeaf(std::move(objects));
        }
        else
        {
            return BecomeABranch(box, std::move(objects));
        }
    }

    unsigned int KDTreeNode::BeacomeALeaf(std::vector<GeometryObject*>&& objects)
    {
        m_leaf = true;
        m_objects = new std::vector<GeometryObject*>(std::move(objects));

        return 1;
    }

    unsigned int KDTreeNode::BecomeABranch(const bounding_box3& box, std::vector<GeometryObject*>&& objects)
    {
        m_split_axis = GetSplittingAxis(box, objects);
        //SortObjects(objects);

        m_split_coord = FindSplittingPlane(box, m_split_axis, objects);
        unsigned int depth = GenerateSubNodes(box, std::move(objects));

        return depth;
    }

    unsigned int KDTreeNode::GetSplittingAxis(const bounding_box3& box,
                                              const std::vector<GeometryObject*>& objects) const
    {
#if true
        auto diff = box.max_corner() - box.min_corner();

        unsigned int max_axis = 0;
        for (unsigned int i = 1; i < diff.dimensions(); i++)
        {
            if (diff[i] > diff[max_axis])
                max_axis = i;
        }

        return max_axis;
#else

		std::size_t bestAxis = 0;
		std::size_t bestSplitCount = objects.size() * 2;

		for (std::size_t proposedAxis = 0; proposedAxis < 3; proposedAxis++)
		{
			bounding_box left_box, right_box;
			box.Split(proposedAxis, FindSplittingPlane(box, proposedAxis, objects), left_box, right_box);

			std::size_t left_objects = 0;
			std::size_t right_objects = 0;

			for (auto & object : objects)
			{
				if (object->bounding_box().Intersects(left_box))
					left_objects++;
				if (object->bounding_box().Intersects(right_box))
					right_objects++;
			}

			if (left_objects + right_objects < bestSplitCount)
			{
				bestAxis = proposedAxis;
				bestSplitCount = left_objects + right_objects;
			}
		}

		return bestAxis;
#endif
    }


    void KDTreeNode::SortObjects(std::vector<GeometryObject*>& objects)
    {
        std::sort(objects.begin(), objects.end(),
                  [&](const auto& obj0, const auto& obj1)
                  {
                      return obj0->bounding_box().min_corner()[m_split_axis] < obj1->bounding_box().min_corner()[
                          m_split_axis];
                  });
    }


    space_real KDTreeNode::FindSplittingPlane(const bounding_box3& box, unsigned int axis,
                                              const std::vector<GeometryObject*>& objects) const
    {
        return (box.min_corner()[axis] + box.max_corner()[axis]) * space_real(0.5);

        //auto split_index = sorted_objects->size() / 2;
        //return (*sorted_objects)[split_index]->bounding_box().min_corner()[m_split_axis];
    }

    unsigned int KDTreeNode::GenerateSubNodes(const bounding_box3& box, std::vector<GeometryObject*>&& objects)
    {
        bounding_box3 left_box, right_box;
        box.Split(m_split_axis, m_split_coord, left_box, right_box);

        std::vector<GeometryObject*> left_objects;
        std::vector<GeometryObject*> right_objects;
        left_objects.reserve(objects.size());
        right_objects.reserve(objects.size());

        for (auto& object : objects)
        {
            if (!object->GetBoundsWithinBounds(left_box).empty())
                left_objects.push_back(object);
            if (!object->GetBoundsWithinBounds(right_box).empty())
                right_objects.push_back(object);
        }

        if ((left_objects.size() + right_objects.size() + 12 > objects.size() * 2)
            /*|| left_objects->empty() || right_objects->empty()*/) // doesn't worth to split
        {
            return BeacomeALeaf(std::move(objects));
        }
        else
        {
            //right_objects.erase(
            //	std::remove_if(
            //		std::begin(right_objects),
            //		std::end(right_objects),
            //		[&](const auto object)
            //			{ return std::none_of(std::begin(left_objects), std::end(left_objects), [&](const auto object2) { return object == object2; }); })
            //	, std::end(right_objects)
            //	);

            m_leaf = false;
            m_subnodes[0] = new KDTreeNode();
            m_subnodes[1] = new KDTreeNode();

            unsigned int left_depth = m_subnodes[0]->Build(left_box, std::move(left_objects));
            unsigned int right_depth = m_subnodes[1]->Build(right_box, std::move(right_objects));

            return std::max(left_depth, right_depth) + 1;
        }
    }
}
