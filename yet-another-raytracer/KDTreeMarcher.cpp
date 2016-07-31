#include "KDTreeMarcher.h"
#include"KDTreeNode.h"


KDTreeMarcher::KDTreeMarcher(const bounding_box3 & scene_box, const KDTreeNode * root_node, unsigned int max_depth)
	: m_scene_box(scene_box)
	, m_root_node(root_node)
	, m_current_leaf(nullptr)
	, m_failed_in_constructor(false)
	, m_dist_near(0)
	, m_dist_far(0)
{
	m_stack.reserve(max_depth);
}


KDTreeMarcher::~KDTreeMarcher(void)
{
}

void KDTreeMarcher::Restart(const ray3 & ray, space_real near, space_real far)
{
	m_ray = ray;
	auto box_hit = m_scene_box.IntersectsRay(m_ray);

	m_failed_in_constructor = !box_hit.occurred();
	if (m_failed_in_constructor)
		return;

	space_real biased_near = std::max(box_hit.near(), near);
	space_real biased_far = std::min(box_hit.far(), far);

	m_failed_in_constructor = biased_near > biased_far;
	if (m_failed_in_constructor)
		return;

	m_stack.clear();
	m_stack.push_back(KDTraverseFrame(biased_near, biased_far, m_root_node));
}

const std::vector<GeometryObject*> * KDTreeMarcher::GetCurrentObjects()
{
	return m_current_leaf->objects();
}

bool KDTreeMarcher::MarcheNext()
{
	if (m_failed_in_constructor)
		return false;

	if (m_stack.empty())
		return false;

	auto & frame = m_stack.back();
	const KDTreeNode * node = frame.node();
	space_real dist_near = frame.near();
	space_real dist_far = frame.far();
	m_stack.pop_back();

	while (!node->leaf())
	{
		auto axis = node->split_axis();
		space_real dist_split = (node->split_coord() - m_ray.origin()[axis]) * m_ray.inversed_direction()[axis];

		if (dist_split <= dist_near)
		{
			node = node->GetFarSubnode(m_ray.direction());
		} 
		else if (dist_split >= dist_far)
		{
			node = node->GetNearSubnode(m_ray.direction());
		}
		else
		{
			m_stack.push_back(KDTraverseFrame(dist_split, dist_far, node->GetFarSubnode(m_ray.direction())));
			node = node->GetNearSubnode(m_ray.direction());
			dist_far = dist_split;
		}
	}

	m_dist_near = dist_near;
	m_dist_far = dist_far;
	m_current_leaf = node;
	return true;
}

bool KDTreeMarcher::IsCorrectIntersectionForCurrentState(space_real dist) const
{
	return dist <= m_dist_far; // we don't check m_dist_near because all closer intersections were found previously
}
