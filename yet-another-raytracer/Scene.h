#pragma once

#include "GeometryObject.h"
#include "Camera.h"
#include "LightSource.h"
#include <vector>
#include <memory>
#include <string>

class Scene
{
public:

	Scene(void)
		: m_viewport_width(640)
		, m_viewport_height(480)
		, m_max_trace_depth(4)
		, m_camera(new Camera())
		, m_objects(new ObjectCollection())
		, m_lights(new LightSourceCollection())
	{
	}

	~Scene(void)
	{
	}


	unsigned int viewport_width() const { return m_viewport_width; }
	void viewport_width(unsigned int value) { m_viewport_width = value; }

	unsigned int viewport_height() const { return m_viewport_height; }
	void viewport_height(unsigned int value) { m_viewport_height = value; }

	unsigned int max_trace_depth() const { return m_max_trace_depth; }
	void max_trace_depth(unsigned int val) { m_max_trace_depth = val; }

	const std::string & output_filename() const { return m_output_filename; }
	void output_filename(const std::string &  val) { m_output_filename = val; }

	const std::shared_ptr<Camera> & camera() const { return m_camera; }
	void camera(const std::shared_ptr<Camera> & value) { m_camera = value; }

	std::shared_ptr<ObjectCollection> & objects() { return m_objects; }
	const std::shared_ptr<ObjectCollection> & objects() const { return m_objects; }

	const std::shared_ptr<LightSourceCollection> & lights() const { return m_lights; }
	void lights(std::shared_ptr<LightSourceCollection> val) { m_lights = val; }

private:
	unsigned int m_viewport_width;
	unsigned int m_viewport_height;
	unsigned int m_max_trace_depth;

	std::string m_output_filename; // it's not good place for it.

	std::shared_ptr<Camera> m_camera;

	std::shared_ptr<ObjectCollection> m_objects;
	std::shared_ptr<LightSourceCollection> m_lights;
	
};

