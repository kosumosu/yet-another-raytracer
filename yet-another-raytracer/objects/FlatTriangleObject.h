#pragma once

#include <utility>

#include "GeometryObject.h"

namespace objects
{
	class FlatTriangleObject : public GeometryObject
	{
		using matrix4x3 = math::base_matrix<space_real, 4, 3>;

		const vector3 _vertex0;
		const vector3 _edge01;
		const vector3 _edge02;
		const vector3 _normal;

		const uvs_t _uvs0;
		const uvs_t _uvs_edge01;
		const uvs_t _uvs_edge02;

		const matrix4x3 worldToLocal;

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
			, worldToLocal(computeWorldToLocal(vert0, vert1, vert2, _edge01, _edge02))
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

	static matrix4x3 computeWorldToLocal(
		const vector3 &vert0,
		const vector3 &vert1,
		const vector3 &vert2,
		const vector3 &e1,
		const vector3 &e2
	) {
			const auto normal = math::cross(e1, e2);
			matrix4x3 worldToLocal = matrix4x3::zero();

			if (fabs(normal[0]) > fabs(normal[1]) && fabs(normal[0]) > fabs(normal[2])) {
				worldToLocal[0][0] = 0.0f;
				worldToLocal[1][0] = 0.0f;
				worldToLocal[2][0] = 1.0f;
				worldToLocal[0][1] = e2[2] / normal[0];
				worldToLocal[1][1] = -e1[2] / normal[0];
				worldToLocal[2][1] = normal[1] / normal[0];
				worldToLocal[0][2] = -e2[1] / normal[0];
				worldToLocal[1][2] = e1[1] / normal[0];
				worldToLocal[2][2] = normal[2] / normal[0];
				worldToLocal[0][3] = math::cross(vert2, vert0)[0] / normal[0];
				worldToLocal[1][3] = -math::cross(vert1, vert0)[0] / normal[0];
				worldToLocal[2][3] = -math::dot(vert0, normal) / normal[0];
			} else if (fabs(normal[1]) > fabs(normal[2])) {
				worldToLocal[0][0] = -e2[2] / normal[1];
				worldToLocal[1][0] = e1[2] / normal[1];
				worldToLocal[2][0] = normal[0] / normal[1];
				worldToLocal[0][1] = 0.0f;
				worldToLocal[1][1] = 0.0f;
				worldToLocal[2][1] = 1.0f;
				worldToLocal[0][2] = e2[0] / normal[1];
				worldToLocal[1][2] = -e1[0] / normal[1];
				worldToLocal[2][2] = normal[2] / normal[1];
				worldToLocal[0][3] = math::cross(vert2, vert0)[1] / normal[1];
				worldToLocal[1][3] = -math::cross(vert1, vert0)[1] / normal[1];
				worldToLocal[2][3] = -math::dot(vert0, normal) / normal[1];
			} else if (fabs(normal[2]) > 0.0f) {
				worldToLocal[0][0] = e2[1] / normal[2];
				worldToLocal[1][0] = -e1[1] / normal[2];
				worldToLocal[2][0] = normal[0] / normal[2];
				worldToLocal[0][1] = -e2[0] / normal[2];
				worldToLocal[1][1] = e1[0] / normal[2];
				worldToLocal[2][1] = normal[1] / normal[2];
				worldToLocal[0][2] = 0.0f;
				worldToLocal[1][2] = 0.0f;
				worldToLocal[2][2] = 1.0f;
				worldToLocal[0][3] = math::cross(vert2, vert0)[2] / normal[2];
				worldToLocal[1][3] = -math::cross(vert1, vert0)[2] / normal[2];
				worldToLocal[2][3] = -math::dot(vert0, normal) / normal[2];
			} else {
				// This triangle is degenerate. Set the transformation matrix to
				// the 0 matrix, which in turn will yield 0 values in places that
				// intersection calculations will detect and treat as a miss.
				worldToLocal[0][0] = 0.0f;
				worldToLocal[1][0] = 0.0f;
				worldToLocal[2][0] = 0.0f;
				worldToLocal[0][1] = 0.0f;
				worldToLocal[1][1] = 0.0f;
				worldToLocal[2][1] = 0.0f;
				worldToLocal[0][2] = 0.0f;
				worldToLocal[1][2] = 0.0f;
				worldToLocal[2][2] = 0.0f;
				worldToLocal[0][3] = 0.0f;
				worldToLocal[1][3] = 0.0f;
				worldToLocal[2][3] = 0.0f;
			}
			return worldToLocal;
		}
	};
}