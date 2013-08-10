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
