#pragma once

#include <functional>
#include <ostream>

class RayIntegrator;
class Film;
class Scene;

namespace renderers
{
	template <class TAccelerator>
	class IRenderer
	{
	public:
		IRenderer() = default;
		IRenderer(IRenderer&&) = default;
		IRenderer(const IRenderer& other) = delete;
		IRenderer& operator=(const IRenderer&) = delete;
		IRenderer& operator=(IRenderer&&) = default;

		virtual ~IRenderer() = default;

		virtual void Render(Film& film, const Scene& scene, const TAccelerator& accelerator) const = 0;

		virtual void PrintStats(std::wostream& stream) const = 0;
	};
};