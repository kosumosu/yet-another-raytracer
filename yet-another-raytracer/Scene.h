#pragma once

#include "GeometryObject.h"
#include "Camera.h"
#include "LightSource.h"
#include <vector>
#include <memory>
#include <string>
#include <map>
#include "Texture.h"

using MaterialCollection = std::map<std::string, std::shared_ptr<Material>>;
using MapCollection = std::map<std::string, std::shared_ptr<Texture>>;

class Scene
{
public:

	Scene(void)
		: _viewport_width(640)
		, _viewport_height(480)
		, _cropX(0)
		, _cropY(0)
		, _cropWidth(0)
		, _cropHeight(0)
		, _max_trace_depth(4)
		, _camera(std::make_shared<Camera>())
		, _environmentColor(color_rgbx::zero())
		, _samplesPerPixel(1) { }


	unsigned int viewport_width() const { return _viewport_width; }
	void viewport_width(unsigned int value) { _viewport_width = value; }

	unsigned int viewport_height() const { return _viewport_height; }
	void viewport_height(unsigned int value) { _viewport_height = value; }

	unsigned int max_trace_depth() const { return _max_trace_depth; }
	void max_trace_depth(unsigned int val) { _max_trace_depth = val; }

	const std::string & output_filename() const { return _output_filename; }
	void output_filename(const std::string & val) { _output_filename = val; }

	const std::shared_ptr<Camera> & camera() const { return _camera; }
	void camera(const std::shared_ptr<Camera> & value) { _camera = value; }

	ObjectCollection & objects() { return _objects; }
	const ObjectCollection & objects() const { return _objects; }

	const LightSourceCollection & lights() const { return _lights; }
	LightSourceCollection & lights() { return _lights; }

	color_rgbx getEnvironmentColor() const { return _environmentColor; }
	void setEnvironmentColor(const color_rgbx & color) { _environmentColor = color; }

	std::size_t getSamplesPerPixel() const { return _samplesPerPixel; }
	void setSamplesPerPixel(std::size_t samplesPerPixel) { _samplesPerPixel = samplesPerPixel; }


	unsigned getCropX() const { return _cropX; }
	void setCropX(const unsigned cropX) { _cropX = cropX; }

	unsigned getCropY() const { return _cropY; }
	void setCropY(const unsigned cropY) { _cropY = cropY; }

	unsigned getCropWidth() const { return _cropWidth; }
	void setCropWidth(const unsigned cropWidth) { _cropWidth = cropWidth; }

	unsigned getCropHeight() const { return _cropHeight; }
	void setCropHeight(const unsigned cropHeight) { _cropHeight = cropHeight; }

	MaterialCollection & getMaterials() { return _materials; }
	const MaterialCollection & getMaterials() const { return _materials; }

	MapCollection & getMaps() { return _maps; }
	const MapCollection & getMaps() const { return _maps; }

private:
	unsigned int _viewport_width;
	unsigned int _viewport_height;
	unsigned int _cropX;
	unsigned int _cropY;
	unsigned int _cropWidth;
	unsigned int _cropHeight;

	unsigned int _max_trace_depth;

	std::string _output_filename; // it's not good place for it.

	std::shared_ptr<Camera> _camera;

	ObjectCollection _objects;
	LightSourceCollection _lights;
	MaterialCollection _materials;
	MapCollection _maps;

	color_rgbx _environmentColor;
	std::size_t _samplesPerPixel;
};
