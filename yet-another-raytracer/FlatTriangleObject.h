#pragma once

#include <utility>

#include "GeometryObject.h"

class FlatTriangleObject : public GeometryObject
{
public:
	explicit FlatTriangleObject()
		: _vertex0(vector3::zero())
		, _vertex1(vector3::zero())
		, _vertex2(vector3::zero())
		, _normal(vector3::zero())
		, _uvs0({vector2(0, 0)})
		, _uvs1({vector2(0, 0)})
		, _uvs2({vector2(0, 0)}) {}

	FlatTriangleObject(vector3 vert0, vector3 vert1, vector3 vert2, uvs_t uvs0, uvs_t uvs1, uvs_t uvs2)
		: _vertex0(std::move(vert0))
		, _vertex1(std::move(vert1))
		, _vertex2(std::move(vert2))
		, _normal(vector3::zero())
		, _uvs0(std::move(uvs0))
		, _uvs1(std::move(uvs1))
		, _uvs2(std::move(uvs2))
	{
		calculate_normal();
	}

	Hit FindHit(const ray3 & ray, space_real minDistance, space_real maxDistance) const override;

	bool DoesHit(const ray3 & ray, space_real minDistance, space_real maxDistance) const override;


	const vector3 & vertex0() const { return _vertex0; }
	void vertex0(const vector3 & val) { _vertex0 = val; }

	const vector3 & vertex1() const { return _vertex1; }
	void vertex1(const vector3 & val) { _vertex1 = val; }

	const vector3 & vertex2() const { return _vertex2; }
	void vertex2(const vector3 & val) { _vertex2 = val; }

	void PrepareForRendering() override;

	bounding_box3 GetBoundsWithinBounds(const bounding_box3 & box) const override;
	space_real GetOneSidedSurfaceArea() const override;
	math::random_sample<surface_point, space_real> PickRandomPointOnSurface(math::Sampler<space_real> & sampler) const override;

	[[nodiscard]] std::optional<math::random_sample<surface_point, space_real>> PickRandomPointOnSurfaceForLighting(
		const vector3& illuminatedPointOnSelf,
		math::Sampler<space_real>& sampler) const override;
private:
	vector3 _vertex0;
	vector3 _vertex1;
	vector3 _vertex2;
	vector3 _normal;

	uvs_t _uvs0;
	uvs_t _uvs1;
	uvs_t _uvs2;

	void calculate_normal();
	space_real GetPreciseOneSidedSurfaceArea() const;
};
