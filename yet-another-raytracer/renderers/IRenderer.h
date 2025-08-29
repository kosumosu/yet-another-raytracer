#pragma once

#include <functional>
#include <stop_token>

class RayIntegrator;
class Film;
class Scene;

namespace renderers
{
	struct Rect
	{
		uint_vector2 top_left;
		uint_vector2 size;
	};

	template <class TRayIntegrator>
	class IRenderer
	{
	public:
		using ray_integrator_factory_t = std::function<TRayIntegrator()>;

		IRenderer() = default;
		IRenderer(IRenderer&&) = default;
		IRenderer(const IRenderer& other) = delete;
		IRenderer& operator=(const IRenderer&) = delete;
		IRenderer& operator=(IRenderer&&) = default;

		virtual ~IRenderer() = default;

		virtual void Render(
			Film& film,
			const Rect& rectToRender,
			const Camera& camera,
			std::size_t samplesPerPixel,
			ray_integrator_factory_t rayIntegratorFactory,
			uint32_t seed,
			const std::stop_token& stopToken
			) const = 0;

		virtual void PrintStats(std::wostream& stream) const = 0;
	};
};