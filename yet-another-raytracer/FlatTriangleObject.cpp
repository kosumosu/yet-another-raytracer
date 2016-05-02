#include "FlatTriangleObject.h"

#include <limits>

#define EPSILON std::numeric_limits<space_real>::min() * space_real(16.0)

FlatTriangleObject::FlatTriangleObject(void)
{
}


FlatTriangleObject::~FlatTriangleObject(void)
{
}

// Implementation of "Fast, minimum storage ray/triangle intersection" by Tomas Moller & Ben Trumbore (MT97)
Hit FlatTriangleObject::FindHit( const Ray & ray ) const
{
	auto edge1 = m_vertex1 - m_vertex0;
	auto edge2 = m_vertex2 - m_vertex0;

	auto p = math::cross(ray.direction(), edge2);

	space_real det = math::dot(p, edge1);

	//if (det > -EPSILON && det < EPSILON)
	if (det == space_real(0.0))
		return Hit();

	space_real inv_det = space_real(1.0) / det;

	auto t = ray.origin() - m_vertex0;

	space_real u = math::dot(t, p) * inv_det;
	if (u < space_real(0.0) || u > space_real(1.0))
		return Hit();

	auto q = math::cross(t, edge1);

	space_real v = math::dot(q, ray.direction()) * inv_det;

	if(v < space_real(0.0) || u + v > space_real(1.0))
		return Hit();

	space_real dist = math::dot(q, edge2) * inv_det;

	if (dist <= space_real(0.0))
		return Hit();

	space_real w = space_real(1) - u - v;

	auto hit_point = w * m_vertex0 + u * m_vertex1 + v * m_vertex2;

	return Hit(hit_point, m_normal * -math::sign(math::dot(m_normal, ray.direction())), this, dist);
}

bool FlatTriangleObject::DoesHit( const Ray & ray ) const
{
	auto edge1 = m_vertex1 - m_vertex0;
	auto edge2 = m_vertex2 - m_vertex0;

	auto p = math::cross(ray.direction(), edge2);

	space_real det = math::dot(p, edge1);

	//if (det > -EPSILON && det < EPSILON)
	if (det == space_real(0.0))
		return false;

	space_real inv_det = space_real(1.0) / det;

	auto t = ray.origin() - m_vertex0;

	space_real u = math::dot(t, p) * inv_det;
	if (u < space_real(0.0) || u > space_real(1.0))
		return false;

	auto q = math::cross(t, edge1);

	space_real v = math::dot(q, ray.direction()) * inv_det;

	if(v < space_real(0.0) || u + v > space_real(1.0))
		return false;

	space_real dist = math::dot(q, edge2) * inv_det;

	return dist > space_real(0.0);
}

void FlatTriangleObject::calculate_normal()
{
	m_normal = math::normalize(math::cross(m_vertex1 - m_vertex0, m_vertex2 - m_vertex0));
}


void FlatTriangleObject::PrepareForRendering()
{
	calculate_normal();

	BoundingBox bbox;

	bbox.Include(m_vertex0);
	bbox.Include(m_vertex1);
	bbox.Include(m_vertex2);

	bbox.ExtendForEpsilon();

	bounding_box(bbox);
}

space_real find_intersection(space_real normal0, space_real normal1, space_real normal2, space_real coord0, space_real coord1, space_real d)
{
	return -(normal0 * coord0 + normal1 * coord1 + d) / normal2;
}

template <unsigned int axis>
void include_in_box_if_valid(BoundingBox & refinedBox, const BoundingBox & enclosingBox, const vector3 & point)
{
	if (enclosingBox.min_corner()[axis] <= point[axis] && point[axis] <= enclosingBox.max_corner()[axis])
	{
		refinedBox.Include(point);
	}
}

template <unsigned int axis0, unsigned int axis1, unsigned int axis2>
void refine_box_for_edge(BoundingBox & refinedBox, const BoundingBox & enclosingBox, const vector3 normal, space_real d, const vector3 & corner0, const vector3 & corner1)
{
	vector3 vec;
	vec[axis0] = corner0[axis0];
	vec[axis1] = corner1[axis1];
	vec[axis2] = find_intersection(normal[axis0], normal[axis1], normal[axis2], corner0[axis0], corner1[axis1], d);
	include_in_box_if_valid<axis2>(refinedBox, enclosingBox, vec);
}

template <unsigned int axis0, unsigned int axis1, unsigned int axis2>
void refine_box_for_axis(BoundingBox & refinedBox, const BoundingBox & enclosingBox, const vector3 normal, space_real d)
{
	refine_box_for_edge<axis0, axis1, axis2>(refinedBox, enclosingBox, normal, d, enclosingBox.min_corner(), enclosingBox.min_corner());
	refine_box_for_edge<axis0, axis1, axis2>(refinedBox, enclosingBox, normal, d, enclosingBox.min_corner(), enclosingBox.max_corner());
	refine_box_for_edge<axis0, axis1, axis2>(refinedBox, enclosingBox, normal, d, enclosingBox.max_corner(), enclosingBox.min_corner());
	refine_box_for_edge<axis0, axis1, axis2>(refinedBox, enclosingBox, normal, d, enclosingBox.max_corner(), enclosingBox.max_corner());
}


BoundingBox FlatTriangleObject::GetBoundsWithinBounds(const BoundingBox & box) const
{
	auto d = -math::dot(m_normal, m_vertex0);

	BoundingBox refinedBox;
	// Along Z axis
	refine_box_for_axis<0, 1, 2>(refinedBox, box, m_normal, d);
	// Along Y axis
	refine_box_for_axis<0, 2, 1>(refinedBox, box, m_normal, d);
	// Along X axis
	refine_box_for_axis<1, 2, 0>(refinedBox, box, m_normal, d);

	refinedBox.Intersect(bounding_box());

	return refinedBox;
}
