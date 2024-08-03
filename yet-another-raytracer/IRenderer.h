#pragma once

#include <ostream>

class RayIntegrator;
class Film;
class Scene;

namespace renderers
{
	using area_finished_reporter_t = std::function<void(const Film& result)>;
	using area_started_reporter_t = std::function<area_finished_reporter_t(const uint_vector2& top_left, const uint_vector2& bottom_right)>;


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

		virtual void Render(Film& film, const Scene& scene, const TAccelerator& accelerator, area_started_reporter_t report_area_started) const = 0;

		virtual void PrintStats(std::wostream& stream) const = 0;
	};
};