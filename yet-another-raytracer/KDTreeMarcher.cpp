#include "KDTreeMarcher.h"



KDTreeMarcher::KDTreeMarcher(const Ray & ray, const BoundingBox & scene_box, const KDTreeNode * root_node, space_real near, space_real far, unsigned int max_depth)
	: m_ray(ray)
{
	m_stack.reserve(max_depth);

	auto box_hit = scene_box.IntersectsRay(ray);

	m_failed_in_constructor = !box_hit.occurred();
	if (m_failed_in_constructor)
		return;

	space_real biased_near = std::max(box_hit.near(), near);
	space_real biased_far = std::min(box_hit.far(), far);

	m_failed_in_constructor = biased_near > biased_far;
	if (m_failed_in_constructor)
		return;

	m_stack.push_back(KDTraverseFrame(biased_near, biased_far, root_node));
}


KDTreeMarcher::~KDTreeMarcher(void)
{
}

const ObjectCollection * KDTreeMarcher::GetCurrentObjects()
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

	m_current_leaf = node;
	return true;
}
