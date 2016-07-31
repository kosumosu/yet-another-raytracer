#pragma once

#include "GeometryObject.h"

class FlatTriangleObject : public GeometryObject
{
public:
	FlatTriangleObject(void);
	FlatTriangleObject(const vector3 & vert0, const vector3 & vert1, const vector3 & vert2)
		: m_vertex0(vert0)
		, m_vertex1(vert1)
		, m_vertex2(vert2)
	{
		calculate_normal();
	}

	virtual ~FlatTriangleObject(void);

	Hit FindHit(const ray3 & ray, space_real minDistance, space_real maxDistance) const override;

	bool DoesHit(const ray3 & ray, space_real minDistance, space_real maxDistance) const override;


	const vector3 & vertex0() const { return m_vertex0; }
	void vertex0(const vector3 & val) { m_vertex0 = val; }

	const vector3 & vertex1() const { return m_vertex1; }
	void vertex1(const vector3 & val) { m_vertex1 = val; }

	const vector3 & vertex2() const { return m_vertex2; }
	void vertex2(const vector3 & val) { m_vertex2 = val; }

	void PrepareForRendering() override;

	bounding_box3 GetBoundsWithinBounds(const bounding_box3 & box) const override;
	space_real GetOneSidedSurfaceArea() const override;
	math::random_sample<vector3, space_real> PickRandomPointOnSurface(math::UniformRandomBitGenerator<unsigned int> & engine) const override;

private:
	vector3 m_vertex0;
	vector3 m_vertex1;
	vector3 m_vertex2;
	vector3 m_normal;

	void calculate_normal();
	space_real GetPreciseOneSidedSurfaceArea() const;
};

