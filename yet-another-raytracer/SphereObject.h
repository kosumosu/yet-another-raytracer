#pragma once

#include "TransformableGeometryObject.h"

class SphereObject : public TransfromableGeometryObject
{
public:
	SphereObject();
	virtual ~SphereObject(void);

	Hit FindHit( const ray3 & ray, space_real minDistance, space_real maxDistance) const override;

	bool DoesHit( const ray3 & ray, space_real minDistance, space_real maxDistance) const override;

	vector3 center() const { return m_center; }
	void center(const vector3 & val) { m_center = val; }

	space_real radius() const { return m_radius; }
	void radius(space_real val) { m_radius = val; }

	bounding_box3 GetBoundsWithinBounds(const bounding_box3 & box) const override;

	void PrepareForRendering() override;

	space_real GetOneSidedSurfaceArea() const override;
	math::random_sample<surface_point, space_real> PickRandomPointOnSurface(math::UniformRandomBitGenerator<random_int_t> & engine) const override;

	[[nodiscard]] std::optional<math::random_sample<surface_point, space_real>> PickRandomPointOnSurfaceForLighting(
		const vector3& illuminatedPointOnSelf,
		math::UniformRandomBitGenerator<random_int_t>& engine) const override;
private:
	vector3 m_center;
	space_real m_radius;
};

