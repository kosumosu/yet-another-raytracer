#pragma once

#include <utility>

#include "GeometryObject.h"

class FlatTriangleObject : public GeometryObject
{
    const vector3 _vertex0;
    const vector3 _edge01;
    const vector3 _edge02;
    const vector3 _normal;

    const uvs_t _uvs0;
    const uvs_t _uvs_edge01;
    const uvs_t _uvs_edge02;

public:
	FlatTriangleObject(
            vector3 vert0,
            vector3 vert1,
            vector3 vert2,
            uvs_t uvs0 = {vector2(0, 0)},
            uvs_t uvs1 = {vector2(0, 0)},
            uvs_t uvs2 = {vector2(0, 0)})
		: _vertex0(std::move(vert0))
        , _edge01(vert1 - _vertex0)
        , _edge02(vert2 - _vertex0)
		, _normal(math::normalize(math::cross(vert1 - _vertex0, vert2 - _vertex0)))
		, _uvs0(std::move(uvs0))
		, _uvs_edge01(subtractUVs(uvs1, _uvs0))
		, _uvs_edge02(subtractUVs(uvs2, _uvs0))
	{
	}

	Hit FindHit(const ray3 & ray, space_real minDistance, space_real maxDistance) const override;

	bool DoesHit(const ray3 & ray, space_real minDistance, space_real maxDistance) const override;

	void PrepareForRendering() override;

	bounding_box3 GetBoundsWithinBounds(const bounding_box3 & box) const override;
	space_real GetOneSidedSurfaceArea() const override;
	math::random_sample<surface_point, space_real> PickRandomPointOnSurface(math::Sampler<space_real> & sampler) const override;

	[[nodiscard]] std::optional<math::random_sample<surface_point, space_real>> PickRandomPointOnSurfaceForLighting(
		const vector3& illuminatedPointOnSelf,
		math::Sampler<space_real>& sampler) const override;
private:

    vector3 vertex0() const { return  _vertex0; }
    vector3 vertex1() const { return  _vertex0 + _edge01; }
    vector3 vertex2() const { return  _vertex0 + _edge02; }

	space_real GetPreciseOneSidedSurfaceArea() const;

    template <std::size_t... Indices>
    uvs_t subtractUVs_impl(const uvs_t& uvs0, const uvs_t& uvs1, std::index_sequence<Indices...>)
    {
        return uvs_t{{(uvs0[Indices] - uvs1[Indices])...}};
    }

    uvs_t subtractUVs(const uvs_t& uvs0, const uvs_t& uvs1)
    {
        return subtractUVs_impl(uvs0, uvs1, std::make_index_sequence<UVS_COUNT>());
    }
};
