#include "Types.h"
#include "discrete_distribution.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <map>
#include <tchar.h>

//////////////////////////////////////////////////////////////////////////

template <typename TValue, typename TEngine>
class TestSampler
{
	mutable TEngine engine_;
	mutable std::uniform_real_distribution<TValue> distr_;
public:
	explicit TestSampler(TEngine engine)
		: engine_{ std::move(engine) }
		, distr_{ TValue(0), TValue(1) - getGrain() }
	{
	}

	TValue Get1D() const
	{
		return distr_(engine_);
	}

	math::vector<TValue, 2> Get2D() const
	{
		return { Get1D(), Get1D() };
	}

private:
	TValue getGrain() const
	{
		return TValue(1.0) / (TValue(engine_.max()) - TValue(engine_.min()));
	}

	static float getGrain(const TEngine& engine, float)
	{
		return float(1.0) / (float(engine.max()) - float(engine.min()));
	}

	static double getGrain(const TEngine& engine, double)
	{
		const auto engineSpan = double(engine.max()) - double(engine.min());
		return double(1.0) / (engineSpan * engineSpan);
	}
};

__declspec(noinline)
bool TestFloatVectorLinearRand()
{
	TestSampler<float, std::mt19937_64> sampler { std::mt19937_64{} };
	auto vec1 = math::linearRand(vector3(1, 11, 21), vector3(2, 12, 22), sampler);
	auto vec2 = math::linearRand(vector3(1, 11, 21), vector3(2, 12, 22), sampler);

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
	//const std::tuple<math::vector<float, 2>, math::vector<float, 2>> a = { {1, 2.0f}, {3, 4.0f} };
	//const math::base_matrix<float, 3, 2> mat0 = { {2.0f, 1.0f, 3.0f},  { 6.0f, 4.0f, 5.0f } };
	const math::base_matrix<float, 3, 2> mat1{ math::vector<float, 3> {2.0f, 1.0f, 3.0f},  math::vector<float, 3> { 6.0f, 4.0f, 5.0f } };

	const math::base_matrix<float, 2, 3> mat2{ math::vector<float, 2> { 3, 1 }, math::vector<float, 2> {1, 2}, math::vector<float, 2> {5, 3} };

	const auto res = mat1 * mat2;

	const math::base_matrix<float, 2, 2> mat3{ math::vector<float, 2> {22, 13}, math::vector<float, 2> { 47, 29 } };


	return res == mat3;
}

__declspec(noinline)
bool TestMatInverse()
{
	math::base_matrix<float, 3, 3> mat = math::base_matrix<float, 3, 3>::identity();
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

	return ident == math::base_matrix<float, 3, 3>::identity();
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
		const auto sample = distribution.GetRandomElement([&]() { return distr(engine); });
		const bool isOkItem = std::abs(sample.getPdf() - items.at(sample.getValue())) < items.at(sample.getValue()) * allowedPdfError;
		allOk = allOk && isOkItem;


		bins[sample.getValue()]++;
		auto asd = bins.at(sample.getValue());
	}

	for (const auto & bin : bins)
	{
		const float ratio = (bin.second / float(iterationCount)) / items.at(bin.first);

		const bool isOk = ratio > 0.9f && ratio < 1.1f;
		allOk = allOk && isOk;
	}

	return allOk;
}

__declspec(noinline)
bool TestConstexprVector()
{
	constexpr float arr[] = { 1.0f, 1.0f, 1.0f };
	constexpr math::vector<float, 3> zeroVec = math::vector<float, 3>::zero();
	constexpr math::vector<float, 3> vec1(arr);
	constexpr math::vector<float, 3> vec2 = math::vector<float, 3>::fill(2.0f);

	constexpr bool isCorrect =
		zeroVec[0] == 0.0f && zeroVec[1] == 0.0f && zeroVec[2] == 0.0f
		&& vec1[0] == 1.0f && vec1[1] == 1.0f && vec1[2] == 1.0f
		&& vec2[0] == 2.0f && vec2[1] == 2.0f && vec2[2] == 2.0f;

	return isCorrect;
}

__declspec(noinline)
bool TestConstexprMatrix()
{
	constexpr math::base_matrix<float, 3, 2> mat1{ math::vector<float, 3>{2.0f, 1.0f, 3.0f}, math::vector<float, 3>{ 6.0f, 4.0f, 5.0f } };

	constexpr bool isCorrect =
		mat1[0][0] == 2.0f && mat1[0][1] == 1.0f && mat1[0][2] == 3.0f
		&& mat1[1][0] == 6.0f && mat1[1][1] == 4.0f && mat1[1][2] == 5.0f
		;

	return isCorrect;
}

//////////////////////////////////////////////////////////////////////////

int _tmain(int argc, _TCHAR* argv[])
{
	std::cout << "vec linearRand\t\t" << (TestFloatVectorLinearRand() ? "ok" : "failed") << std::endl;
	std::cout << "vec dot\t\t\t" << (TestFloatVectorDotProduct() ? "ok" : "failed") << std::endl;
	std::cout << "vec constexpr\t\t" << (TestConstexprVector() ? "ok" : "failed") << std::endl;

	std::cout << std::endl;

	std::cout << "mat * mat\t\t" << (TestMatMatMult() ? "ok" : "failed") << std::endl;
	std::cout << "mat inverse\t\t" << (TestMatInverse() ? "ok" : "failed") << std::endl;
	std::cout << "mat translate\t\t" << (TestMatTranslate() ? "ok" : "failed") << std::endl;
	std::cout << "mat scale\t\t" << (TestMatScale() ? "ok" : "failed") << std::endl;
	std::cout << "mat rotate\t\t" << (TestMatRotate() ? "ok" : "failed") << std::endl;
	std::cout << "mat constexpr\t\t" << (TestConstexprMatrix() ? "ok" : "failed") << std::endl;

	std::cout << "discrete_distribution \t" << (TestDiscreteDistribution() ? "ok" : "failed") << std::endl;

	std::cin.clear();
	std::cin.get();

	return 0;
}

