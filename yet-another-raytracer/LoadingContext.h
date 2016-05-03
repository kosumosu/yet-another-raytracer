#pragma once

#include "Scene.h"
#include "Material.h"
#include "Types.h"
#include <memory>

class LoadingContext
{
public:

	LoadingContext(Scene * scene)
		: m_scene(scene)
	{
	}

	~LoadingContext(void)
	{
	}

	Scene * scene() const { return m_scene; }

	const std::shared_ptr<Material> & material() const { return m_material; }
	void material(const std::shared_ptr<Material> & val) { m_material = val; }

	const matrix4 &  transform() const { return m_transform; }
	void transform(const matrix4 & val) { m_transform = val; }

private:
	Scene * m_scene;
	std::shared_ptr<Material> m_material;
	matrix4 m_transform;

};

