#pragma once

#include "GeometryObject.h"

class __declspec(align(MM_ALIGNMENT)) FlatTriangleObject : public GeometryObject
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

	virtual Hit FindHit( const Ray & ray ) const;

	virtual bool DoesHit( const Ray & ray ) const;


	const vector3 & vertex0() const { return m_vertex0; }
	void vertex0(const vector3 & val) { m_vertex0 = val; }

	const vector3 & vertex1() const { return m_vertex1; }
	void vertex1(const vector3 & val) { m_vertex1 = val; }

	const vector3 & vertex2() const { return m_vertex2; }
	void vertex2(const vector3 & val) { m_vertex2 = val; }

	virtual void PrepareForRendering();

	virtual BoundingBox GetBoundsWithinBounds(const BoundingBox & box) const override;

private:
	vector3 m_vertex0;
	vector3 m_vertex1;
	vector3 m_vertex2;
	vector3 m_normal;

	void calculate_normal();
};

