#pragma once

#include <ostream>

class RayIntegrator;
class Film;
class Scene;

class IRenderer
{
public:
	IRenderer() = default;
	IRenderer(IRenderer&&) = default;
	IRenderer(const IRenderer& other) = delete;
	IRenderer& operator=(const IRenderer&) = delete;
	IRenderer& operator=(IRenderer&&) = default;

	virtual ~IRenderer() = default;

	virtual void Render(Film& film, const Scene& scene) const = 0;

	virtual void PrintStats(std::wostream& stream) const = 0;
};
