#pragma once

#include "TransformableGeometryObject.h"

class SphereObject : public TransfromableGeometryObject
{
public:
	

	SphereObject(void);
	virtual ~SphereObject(void);

	virtual Hit FindHit( const Ray & ray ) const override;

	virtual bool DoesHit( const Ray & ray ) const override;

	vector3 center() const { return m_center; }
	void center(const vector3 & val) { m_center = val; }

	space_real radius() const { return m_radius; }
	void radius(space_real val) { m_radius = val; }

	virtual BoundingBox GetBoundsWithinBounds(const BoundingBox & box) const override;

	virtual void PrepareForRendering() override;

private:
	vector3 m_center;
	space_real m_radius;
};

