#pragma once

#include "TransformableGeometryObject.h"

class SphereObject : public TransfromableGeometryObject
{
public:
	

	SphereObject(void);
	virtual ~SphereObject(void);

	Hit FindHit( const Ray & ray, space_real minDistance, space_real maxDistance) const override;

	bool DoesHit( const Ray & ray, space_real minDistance, space_real maxDistance) const override;

	vector3 center() const { return m_center; }
	void center(const vector3 & val) { m_center = val; }

	space_real radius() const { return m_radius; }
	void radius(space_real val) { m_radius = val; }

	BoundingBox GetBoundsWithinBounds(const BoundingBox & box) const override;

	void PrepareForRendering() override;

	space_real GetOneSidedSurfaceArea() const override;
	math::random_sample<vector3, space_real> PickRandomPointOnSurface(math::UniformRandomBitGenerator<unsigned int> & engine) const override;
private:
	vector3 m_center;
	space_real m_radius;
};

