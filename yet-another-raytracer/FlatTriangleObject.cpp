#include "FlatTriangleObject.h"

#include <limits>

constexpr space_real EPSILON = std::numeric_limits<space_real>::min() * space_real(16.0);


template <std::size_t... Indices>
uvs_t interpolateUVs_impl(const uvs_t& uvs0, const uvs_t& uvs1, const uvs_t& uvs2, space_real u, space_real v, space_real w, std::index_sequence<Indices...>)
{
	return uvs_t{{(uvs0[Indices] * w + uvs1[Indices] * u + uvs2[Indices] * v)...}};
}

uvs_t interpolateUVs(const uvs_t& uvs0, const uvs_t& uvs1, const uvs_t& uvs2, space_real u, space_real v, space_real w)
{
	return interpolateUVs_impl(uvs0, uvs1, uvs2, u, v, w, std::make_index_sequence<UVS_COUNT>());
}

// Implementation of "Fast, minimum storage ray/triangle intersection" by Tomas Moller & Ben Trumbore (MT97)
Hit FlatTriangleObject::FindHit(const ray3& ray, space_real minDistance, space_real maxDistance) const
{
	const auto edge1 = _vertex1 - _vertex0;
	const auto edge2 = _vertex2 - _vertex0;

	const auto p = math::cross(ray.direction(), edge2);

	const auto det = math::dot(p, edge1);

	//if (det > -EPSILON && det < EPSILON)
	if (det == space_real(0.0))
		return Hit();

	const space_real inv_det = space_real(1.0) / det;

	const auto t = ray.origin() - _vertex0;

	const space_real u = math::dot(t, p) * inv_det;
	if (u < space_real(0.0) || u > space_real(1.0))
		return Hit();

	const auto q = math::cross(t, edge1);

	const space_real v = math::dot(q, ray.direction()) * inv_det;

	if (v < space_real(0.0) || u + v > space_real(1.0))
		return Hit();

	const space_real dist = math::dot(q, edge2) * inv_det;

	if (dist < minDistance || dist > maxDistance)
		return Hit();

	const space_real w = space_real(1) - u - v;

	const auto hit_point = w * _vertex0 + u * _vertex1 + v * _vertex2;

	return Hit(hit_point, _normal, this, dist, interpolateUVs(_uvs0, _uvs1, _uvs2, u, v, w));
}

bool FlatTriangleObject::DoesHit(const ray3& ray, space_real minDistance, space_real maxDistance) const
{
	const auto edge1 = _vertex1 - _vertex0;
	const auto edge2 = _vertex2 - _vertex0;

	const auto p = math::cross(ray.direction(), edge2);

	const space_real det = math::dot(p, edge1);

	//if (det > -EPSILON && det < EPSILON)
	if (det == space_real(0.0))
		return false;

	const space_real inv_det = space_real(1.0) / det;

	const auto t = ray.origin() - _vertex0;

	const space_real u = math::dot(t, p) * inv_det;
	if (u < space_real(0.0) || u > space_real(1.0))
		return false;

	const auto q = math::cross(t, edge1);

	const space_real v = math::dot(q, ray.direction()) * inv_det;

	if (v < space_real(0.0) || u + v > space_real(1.0))
		return false;

	const space_real dist = math::dot(q, edge2) * inv_det;

	return dist >= minDistance && dist <= maxDistance;
}

void FlatTriangleObject::calculate_normal()
{
	_normal = math::normalize(math::cross(_vertex1 - _vertex0, _vertex2 - _vertex0));
}


void FlatTriangleObject::PrepareForRendering()
{
	calculate_normal();

	bounding_box3 bbox;

	bbox.Include(_vertex0);
	bbox.Include(_vertex1);
	bbox.Include(_vertex2);

	bbox.ExtendForEpsilon();

	bounding_box(bbox);
}

space_real find_intersection(space_real normal0, space_real normal1, space_real normal2, space_real coord0, space_real coord1, space_real d)
{
	return -(normal0 * coord0 + normal1 * coord1 + d) / normal2;
}

template <unsigned int axis>
void include_in_box_if_valid(bounding_box3& refinedBox, const bounding_box3& enclosingBox, const vector3& point)
{
	if (enclosingBox.min_corner()[axis] <= point[axis] && point[axis] <= enclosingBox.max_corner()[axis])
	{
		refinedBox.Include(point);
	}
}

template <unsigned int axis0, unsigned int axis1, unsigned int axis2>
void refine_box_for_edge(
	bounding_box3& refinedBox,
	const bounding_box3& enclosingBox,
	const vector3 normal,
	space_real d,
	const vector3& corner0,
	const vector3& corner1)
{
	vector3 vec = vector3::zero();
	vec[axis0] = corner0[axis0];
	vec[axis1] = corner1[axis1];
	vec[axis2] = find_intersection(normal[axis0], normal[axis1], normal[axis2], corner0[axis0], corner1[axis1], d);
	include_in_box_if_valid<axis2>(refinedBox, enclosingBox, vec);
}

template <unsigned int axis0, unsigned int axis1, unsigned int axis2>
void refine_box_for_axis(bounding_box3& refinedBox, const bounding_box3& enclosingBox, const vector3 normal, space_real d)
{
	refine_box_for_edge<axis0, axis1, axis2>(refinedBox, enclosingBox, normal, d, enclosingBox.min_corner(), enclosingBox.min_corner());
	refine_box_for_edge<axis0, axis1, axis2>(refinedBox, enclosingBox, normal, d, enclosingBox.min_corner(), enclosingBox.max_corner());
	refine_box_for_edge<axis0, axis1, axis2>(refinedBox, enclosingBox, normal, d, enclosingBox.max_corner(), enclosingBox.min_corner());
	refine_box_for_edge<axis0, axis1, axis2>(refinedBox, enclosingBox, normal, d, enclosingBox.max_corner(), enclosingBox.max_corner());
}


bounding_box3 FlatTriangleObject::GetBoundsWithinBounds(const bounding_box3& box) const
{
	auto d = -math::dot(_normal, _vertex0);

	bounding_box3 refinedBox;
	// Along Z axis
	refine_box_for_axis<0, 1, 2>(refinedBox, box, _normal, d);
	// Along Y axis
	refine_box_for_axis<0, 2, 1>(refinedBox, box, _normal, d);
	// Along X axis
	refine_box_for_axis<1, 2, 0>(refinedBox, box, _normal, d);

	refinedBox.Intersect(bounding_box());

	return refinedBox;
}

space_real FlatTriangleObject::GetPreciseOneSidedSurfaceArea() const
{
	return math::length(math::cross(_vertex1 - _vertex0, _vertex2 - _vertex0)) * space_real(0.5);
}

space_real FlatTriangleObject::GetOneSidedSurfaceArea() const
{
	return GetPreciseOneSidedSurfaceArea();
}

math::random_sample<surface_point, space_real> FlatTriangleObject::PickRandomPointOnSurface(math::UniformRandomBitGenerator<random_int_t>& engine) const
{
	std::uniform_real_distribution<space_real> distr;
	const auto rawU = distr(engine);
	const auto rawV = distr(engine);

	const bool isOnTheOtherHalfOfTheParallelogram = rawV + rawU > space_real(1);

	const auto finalU = isOnTheOtherHalfOfTheParallelogram ? space_real(1) - rawU : rawU;
	const auto finalV = isOnTheOtherHalfOfTheParallelogram ? space_real(1) - rawV : rawV;
	const auto finalW = space_real(1) - finalU - finalV;

	const auto finalPoint = _vertex0 * finalW + _vertex1 * finalU + _vertex2 * finalV;
	return math::random_sample<surface_point, space_real>(
		surface_point(finalPoint, _normal, interpolateUVs(_uvs0, _uvs1, _uvs2, finalW, finalU, finalV)),
		space_real(1.0) / GetPreciseOneSidedSurfaceArea(),
		false);
}

std::optional<math::random_sample<surface_point, space_real>> FlatTriangleObject::PickRandomPointOnSurfaceForLighting(
	const vector3& illuminatedPointOnSelf,
	math::UniformRandomBitGenerator<random_int_t>& engine) const
{
	// Since flat triangle can't illuminate itself, just ignore it.
	return std::nullopt;
}
