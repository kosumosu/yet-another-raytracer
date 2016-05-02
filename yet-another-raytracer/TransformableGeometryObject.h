#pragma once

#include "GeometryObject.h"
#include "Types.h"
#include <exception>

class TransfromableGeometryObject : public GeometryObject
{
public:
	

	TransfromableGeometryObject() { }

	TransfromableGeometryObject(const matrix4 & transform)
		: m_transform(transform)
	{
	}

	virtual ~TransfromableGeometryObject(void)
	{
	}
	
	
	void transform(const matrix4 & value)
	{
		m_transform = value;
		m_inverse_transform = math::inverse(m_transform);
		m_normals_transform = math::transpose(m_inverse_transform);
	}

	const matrix4 & transform() const { return m_transform; }
	virtual matrix4 final_transform() const { return m_transform; }

	const matrix4 & normal_transform() const { return m_normals_transform; }
	const matrix4 & inverse_transform() const { return m_inverse_transform; }

private:
	matrix4 m_transform;
	matrix4 m_normals_transform;
	matrix4 m_inverse_transform;

};