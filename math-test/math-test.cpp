#include "stdafx.h"

#include "Types.h"
#include "discrete_distribution.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <map>

//////////////////////////////////////////////////////////////////////////

__declspec(noinline)
bool TestFloatVectorLinearRand()
{
	std::mt19937_64 engine;
	auto vec1 = math::linearRand(vector3(1, 11, 21), vector3(2, 12, 22), engine);
	auto vec2 = math::linearRand(vector3(1, 11, 21), vector3(2, 12, 22), engine);

	bool success = vec1[0] >= 1 && vec1[0] <= 2 && vec1[1] >= 11 && vec1[1] <= 12 && vec1[2] >= 21 && vec1[2] <= 22;
	success &= vec2[0] >= 1 && vec2[0] <= 2 && vec2[1] >= 11 && vec2[1] <= 12 && vec2[2] >= 21 && vec2[2] <= 22;

	success &= vec1[0] != vec2[0] && vec1[1] != vec2[1] && vec1[2] != vec2[2];

	return success;
}


__declspec(noinline)
bool TestFloatVectorDotProduct()
{
	math::vector<float, 3> vec1(1.0, 2.0, 3.0);
	math::vector<float, 3> vec2(4, 5, 6);

	glm::vec3 _vec1(1.0, 2.0, 3.0);
	glm::vec3 _vec2(4, 5, 6);

	auto result = math::dot(vec1, vec2);
	
	auto expected = glm::dot(_vec1, _vec2);

	return result == expected;
}


__declspec(noinline)
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

__declspec(noinline)
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

__declspec(noinline)
bool CompareWithGLM(const math::base_matrix<float, 4, 4> & one, const glm::mat4 & another)
{
	for (size_t j = 0; j < 4; j++)
	{
		for (size_t i = 0; i < 4; i++)
		{
			if (one[j][i] != another[glm::mat4::length_type(i)][glm::mat4::length_type(j)])
				return false;
		}
	}
	return true;
}

__declspec(noinline)
bool TestMatTranslate()
{
	auto translate = math::translate(math::vector<float, 3>(1.0f, 2.0f, 3.0f));

	auto glm_translate = glm::translate(glm::mat4(), glm::vec3(1.0f, 2.0f, 3.0f));

	return CompareWithGLM(translate, glm_translate);
}

__declspec(noinline)
bool TestMatScale()
{
	auto scale = math::scale(math::vector<float, 3>(4.0f, 5.0f, 6.0f));

	auto glm_scale = glm::scale(glm::mat4(), glm::vec3(4.0f, 5.0f, 6.0f));

	return CompareWithGLM(scale, glm_scale);
}

__declspec(noinline)
bool TestMatRotate()
{
	constexpr auto angle = float(math::pi / 2.0);
	auto rotate = math::rotate(math::vector<float, 3>(1.0f, 0.0f, 0.0f), angle);

	auto glm_rotate = glm::rotate(glm::mat4(), angle, glm::vec3(1.0f, 0.0f, 0.0f));

	return CompareWithGLM(rotate, glm_rotate);
}

__declspec(noinline)
bool TestDiscreteDistribution()
{
	bool allOk = true;

	constexpr int iterationCount = 1000;
	constexpr float allowedPdfError = std::numeric_limits<float>::epsilon() * 16.0f;

	std::map<int, float> items = { {10, 0.1f}, {20, 0.2f}, {30, 0.3f}, {40, 0.4f} };
	math::discrete_distribution<int, float> distribution(std::begin(items), std::end(items));

	std::mt19937_64 engine;
	std::uniform_real_distribution<float> distr;
	std::map<int, int> bins;

	for (size_t i = 0; i < iterationCount; i++)
	{
		auto sample = distribution.GetRandomElement([&]() { return distr(engine); });
		bool isOkItem = std::abs(sample.getPdf() - items.at(sample.getValue())) < items.at(sample.getValue()) * allowedPdfError;
		allOk = allOk && isOkItem;
		if (!isOkItem)
		{
			int asdf = 666; // put breakpoint here
		}

		bins[sample.getValue()]++;
	}

	for (const auto & bin : bins)
	{
		float ratio = (bin.second / (float)iterationCount) / items.at(bin.first);

		bool isOk = ratio > 0.9f && ratio < 1.1f;
		allOk = allOk && isOk;
	}

	return allOk;
}

//////////////////////////////////////////////////////////////////////////

int _tmain(int argc, _TCHAR* argv[])
{
	std::cout << "vec linearRand\t\t" << (TestFloatVectorLinearRand() ? "ok" : "failed") << std::endl;
	std::cout << "vec dot\t\t\t" << (TestFloatVectorDotProduct() ? "ok" : "failed") << std::endl;

	std::cout << std::endl;

	std::cout << "mat * mat\t\t" << (TestMatMatMult() ? "ok" : "failed") << std::endl;
	std::cout << "mat inverse\t\t" << (TestMatInverse() ? "ok" : "failed") << std::endl;
	std::cout << "mat translate\t\t" << (TestMatTranslate() ? "ok" : "failed") << std::endl;
	std::cout << "mat scale\t\t" << (TestMatScale() ? "ok" : "failed") << std::endl;
	std::cout << "mat rotate\t\t" << (TestMatRotate() ? "ok" : "failed") << std::endl;
	std::cout << "discrete_distribution \t" << (TestDiscreteDistribution() ? "ok" : "failed") << std::endl;

	std::cin.clear();
	std::cin.get();

	return 0;
}

