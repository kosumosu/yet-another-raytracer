#pragma once

#include "Scene.h"
#include "materials/Material.h"
#include "Types.h"
#include <memory>

class LoadingContext
{
public:

	explicit LoadingContext(Scene * scene)
		: m_scene(scene)
		, m_material(nullptr)
		, m_transform(matrix4::identity()) { }

	~LoadingContext(void)
	{
	}

	Scene * scene() const { return m_scene; }

	const materials::Material * material() const { return m_material; }
	void material(const materials::Material * val) { m_material = val; }

	const matrix4 &  transform() const { return m_transform; }
	void transform(const matrix4 & val) { m_transform = val; }

private:
	Scene * m_scene;
	const materials::Material * m_material;
	matrix4 m_transform;
};

