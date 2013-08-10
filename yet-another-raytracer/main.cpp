// 184-1x-hw3.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Film.h"
#include "Scene.h"
#include "SphereObject.h"
#include "FlatTriangleObject.h"
#include "BlinnMaterial.h"
#include "Renderer.h"
#include "HRTimer.h"
#include "DirectionalLightSource.h"
#include "PointLightSource.h"
#include "SkyLightSource.h"

#include "SceneLoader.h"

#include "Types.h"
#include <iostream>
#include <memory>

#define NO_MIN_MAX
#include <Windows.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


void callback(unsigned int x, unsigned int y, float progress)
{
	char buf[10];
	sprintf_s(buf, "%.2f", progress * 100.0f);
	std::cout << "Done " << buf << "%" << std::endl;
}


void InsertRandomSpheres(Scene & scene, unsigned int count)
{
	color4 zero4;
	color4 one4(1.0f);
	vector3 zero3;
	vector3 one3(1.0f);
	vector3 minus_one3(-1.0f);

	for (unsigned int i = 0; i < count; i++)
	{
		std::shared_ptr<BlinnMaterial> material (new BlinnMaterial());
		material->diffuse(math::linearRand(zero4, one4));
		//material->specular(math::linearRand(zero4, one4));
		//material->shininess(math::linearRand(10.0f, 300.0f));
		//material->emission(math::linearRand(zero4, one4 * 0.2f));

		std::shared_ptr<SphereObject> object(new SphereObject());
		object->radius(math::linearRand(space_real(0.1), space_real(1.0)));
		object->center(math::linearRand(minus_one3 * space_real(2.0), one3 * space_real(2.0)));
		object->material(material);

		scene.objects()->push_back(object);
	}
}

void InsertSkyLights(Scene & scene, unsigned int count)
{
	color4 zero4;
	color4 one4(1.0f);
	vector3 zero3;
	vector3 one3(1.0f);
	vector3 minus_one3(-1.0f);

	color_real intensity_per_light = color_real(5.0) / count;

	for (unsigned int i = 0; i < count; i++)
	{
		std::shared_ptr<DirectionalLightSource> light_source(new DirectionalLightSource());
		light_source->direction(math::sphericalRand<space_real>());
		light_source->color(color4(intensity_per_light));

		scene.lights()->push_back(light_source);
	}
}

void InsertRandomPointLights(Scene & scene, unsigned int count)
{
	color4 zero4;
	color4 one4(1.0f);
	vector3 zero3;
	vector3 one3(1.0f);
	vector3 minus_one3(-1.0f);

	color_real intensity_per_light = color_real(60.0) / count;

	for (unsigned int i = 0; i < count; i++)
	{
		std::shared_ptr<PointLightSource> light_source(new PointLightSource());
		light_source->position(math::linearRand(minus_one3 * space_real(3.0), one3 * space_real(3.0)));
		light_source->color(math::linearRand(zero4, color4(intensity_per_light)));

		scene.lights()->push_back(light_source);
	}
}

void InsertTwoSpheres(Scene & scene)
{
	std::shared_ptr<BlinnMaterial> material1 (new BlinnMaterial());
	material1->diffuse(color4(0.75f, 0.3f, 0.0f, 1.0f));
	material1->specular(color4(0.6f, 0.6f, 0.6f, 1.0f));
	material1->shininess(200.0f);
	material1->emission(color4(0.1f, 0.05f, 0.15f, 1.0f));

	std::shared_ptr<SphereObject> object1(new SphereObject());
	object1->radius(1.0f);
	object1->center(vector3(0.0f, -0.9f, 0.0f));
	object1->material(material1);

	std::shared_ptr<BlinnMaterial> material2 (new BlinnMaterial());
	material2->diffuse(color4(0.3f, 0.75f, 0.0f, 1.0f));
	material2->specular(color4(0.6f, 0.6f, 0.6f, 1.0f));
	material2->shininess(200.0f);
	material2->emission(color4(0.1f, 0.05f, 0.15f, 1.0f));

	std::shared_ptr<SphereObject> object2(new SphereObject());
	object2->radius(1.0f);
	object2->center(vector3(0.0f, 0.9f, 0.0f));
	object2->material(material2);

	scene.objects()->push_back(object1);
	scene.objects()->push_back(object2);
}

void InsertTriangle(Scene & scene)
{
	std::shared_ptr<BlinnMaterial> material1 (new BlinnMaterial());
	material1->diffuse(color4(0.1f, 0.3f, 0.75f, 1.0f));
	material1->specular(color4(0.5f, 0.5f, 0.5f, 1.0f));
	material1->shininess(100.0f);
	material1->emission(color4(0.01f, 0.03f, 0.075f, 1.0f));

	std::shared_ptr<FlatTriangleObject> object1(new FlatTriangleObject());
	object1->material(material1);
	object1->vertex0(vector3(0.0f, 0.0f, 0.0f));
	object1->vertex1(vector3(0.0f, 2.0f, 0.0f));
	object1->vertex2(vector3(0.0f, 0.0f, 1.0f));

	scene.objects()->push_back(object1);
}

void InsertRandomTriangles(Scene & scene, unsigned int count)
{
	color4 zero4;
	color4 one4(1.0);
	vector3 zero3;
	vector3 one3(1.0);
	vector3 minus_one3(-1.0);

	for (unsigned int i = 0; i < count; i++)
	{
		std::shared_ptr<BlinnMaterial> material (new BlinnMaterial());
		material->diffuse(math::linearRand(zero4, one4));
		//material->specular(math::linearRand(zero4, one4));
		//material->shininess(math::linearRand(10.0, 300.0));
		//material->emission(math::linearRand(zero4, one4 * 0.2));

		std::shared_ptr<FlatTriangleObject> object(new FlatTriangleObject());
		object->material(material);
		object->vertex0(math::linearRand(minus_one3 * space_real(2.0), one3 * space_real(2.0)));
		object->vertex1(math::linearRand(minus_one3 * space_real(2.0), one3 * space_real(2.0)));
		object->vertex2(math::linearRand(minus_one3 * space_real(2.0), one3 * space_real(2.0)));

		scene.objects()->push_back(object);
	}
}

void InsertPointLight(Scene & scene)
{
	std::shared_ptr<PointLightSource> light_source(new PointLightSource());
	light_source->position(vector3(2.0, 2.0, 2.0));
	light_source->color(color4(20.0, 20.0, 30.0, 1.0));
	light_source->attenuation(Attenuation(1.0, 0.0, 0.0));

	scene.lights()->push_back(light_source);
}

void InsertDirectionalLight(Scene & scene)
{
	std::shared_ptr<DirectionalLightSource> light_source(new DirectionalLightSource());
	light_source->direction(math::normalize(vector3(1.0, 1.0, 1.0)));
	light_source->color(color4(0.6f, 0.6f, 0.9f, 1.0f));

	scene.lights()->push_back(light_source);
}

void InsertSkyLight(Scene & scene)
{
	std::shared_ptr<SkyLightSource> light_source(new SkyLightSource());
	light_source->color(color4(0.5f, 0.64f, 0.82f, 0.0f));
	light_source->samples(64);

	scene.lights()->push_back(light_source);
}


void InsertCamera( Scene &scene )
{
	scene.camera()->fovy(90.0);
	scene.camera()->position(vector3(4.0, 0.0, 0.0));
	scene.camera()->up(vector3(0.0, 0.0, 1.0));
}



//////////////////////////////////////////////////////////////////////////

void LoadFromFile(Scene & scene, const char * filename)
{
	std::auto_ptr<SceneLoader> loader(SceneLoader::CreateDefault());

	loader->Load(scene, filename);
}

//////////////////////////////////////////////////////////////////////////

std::string GetFileName(const std::string & input)
{
	auto index = input.rfind('\\');

	if (index == std::string::npos)
	{
		return input;
	}
	else
	{
		return input.substr(index + 1);
	}
}

std::string GetFileNameWithoutExtension(const std::string & input)
{
	auto filename = GetFileName(input);

	auto index = filename.rfind('.');

	if (index == std::string::npos)
	{
		return filename;
	}
	else
	{
		auto tmp = filename.substr(0, index);
		return tmp;
	}
}

std::string GetPathWithoutExtension(const std::string & input)
{
	auto index = input.rfind('.');

	if (index == std::string::npos)
	{
		return input;
	}
	else
	{
		auto tmp = input.substr(0, index);
		return tmp;
	}
}

//////////////////////////////////////////////////////////////////////////

bool TestMatMatMult()
{
	math::base_matrix<float, 3, 2> mat1;
	mat1[0][0] = 2.0f;
	mat1[0][1] = 1.0f;
	mat1[0][2] = 3.0f;

	mat1[1][0] = 6.0f;
	mat1[1][1] = 4.0f;
	mat1[1][2] = 5.0f;

	math::base_matrix<float, 2, 3> mat2;
	mat2[0][0] = 3.0f;
	mat2[0][1] = 1.0f;

	mat2[1][0] = 1.0f;
	mat2[1][1] = 2.0f;

	mat2[2][0] = 5.0f;
	mat2[2][1] = 3.0f;

	auto res = mat1 * mat2;

	math::base_matrix<float, 2, 2> mat3;
	mat3[0][0] = 22.0f;
	mat3[0][1] = 13.0f;

	mat3[1][0] = 47.0f;
	mat3[1][1] = 29.0f;

	return res == mat3;
}

bool TestMatInverse()
{
	math::base_matrix<float, 3, 3> mat;
	mat[0][0] = -2.0f;
	mat[0][1] = 2.0f;
	mat[0][2] = 3.0f;

	mat[1][0] = -1.0f;
	mat[1][1] = 1.0f;
	mat[1][2] = 3.0f;

	mat[2][0] = 2.0f;
	mat[2][1] = 0.0f;
	mat[2][2] = -1.0f;

	auto inversed = math::inverse(mat);

	auto ident = mat * inversed;

	return ident == math::base_matrix<float, 3, 3>();
}

bool CompareWithGLM(const math::base_matrix<float, 4, 4> & one, const glm::mat4 & another)
{
	for (size_t j = 0; j < 4; j++)
	{
		for (size_t i = 0; i < 4; i++)
		{
			if (one[j][i] != another[i][j])
				return false;
		}
	}
	return true;
}

bool TestMatTranslate()
{
	auto translate = math::translate(math::base_vector<float, 3>(1.0f, 2.0f, 3.0f));

	auto glm_translate = glm::translate(glm::mat4(), glm::vec3(1.0f, 2.0f, 3.0f));

	return CompareWithGLM(translate, glm_translate);
}

bool TestMatScale()
{
	auto scale = math::scale(math::base_vector<float, 3>(4.0f, 5.0f, 6.0f));

	auto glm_scale = glm::scale(glm::mat4(), glm::vec3(4.0f, 5.0f, 6.0f));

	return CompareWithGLM(scale, glm_scale);
}

bool TestMatRotate()
{
	auto rotate = math::rotate(math::base_vector<float, 3>(1.0f, 0.0f, 0.0f), float(M_PI / 2.0));

	auto glm_rotate = glm::rotate(glm::mat4(), 90.0f, glm::vec3(1.0f, 0.0f, 0.0f));

	return CompareWithGLM(rotate, glm_rotate);
}

int _tmain(int argc, _TCHAR* argv[])
{
	//std::cout << "mat * mat\t\t" << (TestMatMatMult() ? "ok" : "failed") << std::endl;
	//std::cout << "mat inverse\t\t" << (TestMatInverse() ? "ok" : "failed") << std::endl;
	//std::cout << "mat translate\t\t" << (TestMatTranslate() ? "ok" : "failed") << std::endl;
	//std::cout << "mat scale\t\t" << (TestMatScale() ? "ok" : "failed") << std::endl;
	//std::cout << "mat rotate\t\t" << (TestMatRotate() ? "ok" : "failed") << std::endl;

	if (argc < 2)
		return 0;

	Scene scene;

	LoadFromFile(scene, argv[1]);

	std::string image_file = GetPathWithoutExtension(argv[1]) + ".png";

	//InsertCamera(scene);

	//InsertDirectionalLight(scene);
	//InsertSkyLights(scene, 200);
	//InsertRandomPointLights(scene, 7);

	//InsertTwoSpheres(scene);
	//InsertRandomSpheres(scene, 20);
	//InsertTriangle(scene);
	//InsertRandomTriangles(scene, 20);
	//InsertSkyLight(scene);


	Film film(scene.viewport_width(), scene.viewport_height());
	Renderer renderer;
	renderer.callback(callback);

	HRTimer timer;
	timer.Restart();

	renderer.Render(film, scene);

	auto elapsed = timer.Sample();

	std::cout << "Elapsed : " << elapsed << "sec" << std::endl;

	film.SaveAsPng(image_file.c_str());

	ShellExecuteA(NULL, "Open", image_file.c_str(), NULL, NULL, SW_SHOWNORMAL);

	std::cin.clear();
	std::cin.get();

	return 0;
}

