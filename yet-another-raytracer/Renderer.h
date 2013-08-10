#pragma once

#include "RayEvaluator.h"

class Film;
class Scene;

class Renderer
{
public:
	typedef void (*progress_callback)(unsigned int x, unsigned int y, float progress);

	Renderer(void);
	~Renderer(void);

	void Render(Film & film, const Scene & scene) const;

	Renderer::progress_callback callback() const { return m_callback; }
	void callback(Renderer::progress_callback val) { m_callback = val; }

private:
	progress_callback m_callback;

	void PrepareObjects(const ObjectCollection & objects) const;
};

