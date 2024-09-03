#pragma once

#include "Camera.h"
#include "objects/GeometryObject.h"
#include "lights/LightSource.h"
#include "materials/Material.h"
#include "Texture.h"

#include <map>
#include <memory>
#include <string>
#include <vector>

using MaterialCollection = std::map<std::string, std::shared_ptr<materials::Material>>;
using MapCollection = std::map<std::string, std::shared_ptr<Texture>>;

class Scene
{
public:

	Scene()
		: viewportWidth_(640)
		, viewportHeight_(480)
		, cropX_(0)
		, cropY_(0)
		, cropWidth_(0)
		, cropHeight_(0)
		, maxTraceDepth_(4)
		, camera_(std::make_shared<Camera>())
		, environmentColor_(color_rgb::zero())
		, samplesPerPixel_(1)
	{
	}

	[[nodiscard]] unsigned int viewport_width() const { return viewportWidth_; }
	void viewport_width(unsigned int value) { viewportWidth_ = value; }

	[[nodiscard]] unsigned int viewport_height() const { return viewportHeight_; }
	void viewport_height(unsigned int value) { viewportHeight_ = value; }

	[[nodiscard]] unsigned int max_trace_depth() const { return maxTraceDepth_; }
	void max_trace_depth(unsigned int val) { maxTraceDepth_ = val; }

	[[nodiscard]] const std::string& output_filename() const { return outputFilename_; }
	void output_filename(const std::string& val) { outputFilename_ = val; }

	[[nodiscard]] const std::shared_ptr<Camera>& camera() const { return camera_; }
	void camera(const std::shared_ptr<Camera>& value) { camera_ = value; }

	ObjectCollection& objects() { return objects_; }
	[[nodiscard]] const ObjectCollection& objects() const { return objects_; }

	[[nodiscard]] const lights::LightSourceCollection& lights() const { return lights_; }
	lights::LightSourceCollection& lights() { return lights_; }

	[[nodiscard]] color_rgb getEnvironmentColor() const { return environmentColor_; }
	void setEnvironmentColor(const color_rgb& color) { environmentColor_ = color; }

	[[nodiscard]] std::size_t getSamplesPerPixel() const { return samplesPerPixel_; }
	void setSamplesPerPixel(std::size_t samplesPerPixel) { samplesPerPixel_ = samplesPerPixel; }


	[[nodiscard]] unsigned getCropX() const { return cropX_; }
	void setCropX(const unsigned cropX) { cropX_ = cropX; }

	[[nodiscard]] unsigned getCropY() const { return cropY_; }
	void setCropY(const unsigned cropY) { cropY_ = cropY; }

	[[nodiscard]] unsigned getCropWidth() const { return cropWidth_; }
	void setCropWidth(const unsigned cropWidth) { cropWidth_ = cropWidth; }

	[[nodiscard]] unsigned getCropHeight() const { return cropHeight_; }
	void setCropHeight(const unsigned cropHeight) { cropHeight_ = cropHeight; }

	MaterialCollection& getMaterials() { return materials_; }
	[[nodiscard]] const MaterialCollection& getMaterials() const { return materials_; }

	MapCollection& getMaps() { return maps_; }
	[[nodiscard]] const MapCollection& getMaps() const { return maps_; }

	void PrepareForRendering()
	{
		for (const auto& object : objects_)
		{
			object->PrepareForRendering();
		}
	}

private:
	unsigned int viewportWidth_;
	unsigned int viewportHeight_;
	unsigned int cropX_;
	unsigned int cropY_;
	unsigned int cropWidth_;
	unsigned int cropHeight_;

	unsigned int maxTraceDepth_;

	std::string outputFilename_; // it's not good place for it.

	std::shared_ptr<Camera> camera_;

	ObjectCollection objects_;
	lights::LightSourceCollection lights_;
	MaterialCollection materials_;
	MapCollection maps_;

	color_rgb environmentColor_;
	std::size_t samplesPerPixel_;
};
