#pragma once

#include <functional>
#include <utility>
#include "random_sample.hpp"

template <typename TSample, typename TPdfSample, typename TProbability>
class Distribution
{
public:
	using delta_func = std::function<void(const math::random_sample<TSample, TProbability>& sample)>;

	virtual ~Distribution() = default;

	virtual std::size_t delta_components() const = 0;
	virtual math::random_sample<TSample, TProbability> generate_delta_component_sample() const = 0;
	virtual bool has_non_delta_component() const = 0;
	virtual math::random_sample<TSample, TProbability> generate_non_delta_component_sample() const = 0;
	virtual TProbability evaluate_pdf(const TPdfSample& sample) const = 0; // Probably not needed
	virtual math::random_sample<TSample, TProbability> generate_sample() const = 0;
};


template <typename TSample, typename TPdfSample, typename TProbability>
class FunctionalDistribution : public Distribution<TSample, TPdfSample, TProbability>
{
public:
	using delta_func = std::function<void(const math::random_sample<TSample, TProbability>& sample)>;

	using generate_sample_func = std::function<math::random_sample<TSample, TProbability>()>;
	using evaluate_pdf_func = std::function<TProbability(const TPdfSample& sample)>;

private:
	const std::size_t deltaComponentCount_;
	const bool hasNonDeltaComponents_;
	const generate_sample_func generateDeltaComponentSample_;
	const generate_sample_func generateNonDeltaComponentSample_;
	const evaluate_pdf_func evaluatePdf_;
	const generate_sample_func generateSample_;

public:

	FunctionalDistribution()
		: deltaComponentCount_(0)
		, hasNonDeltaComponents_(false)
		, generateNonDeltaComponentSample_(nullptr)
		, evaluatePdf_(zero_evaluate_pdf)
		, generateSample_(nullptr)
	{
	}

	FunctionalDistribution(
		std::size_t deltaComponentCount,
		generate_sample_func generateDeltaComponentSample,
		generate_sample_func generateNonDeltaComponentSample,
		evaluate_pdf_func evaluatePdf,
		generate_sample_func generateSample)
		: deltaComponentCount_(deltaComponentCount)
		, hasNonDeltaComponents_(generateNonDeltaComponentSample != nullptr)
		, generateDeltaComponentSample_(std::move(generateDeltaComponentSample))
		, generateNonDeltaComponentSample_(std::move(generateNonDeltaComponentSample))
		, evaluatePdf_(std::move(evaluatePdf))
		, generateSample_(std::move(generateSample))
	{
	}

	explicit FunctionalDistribution(generate_sample_func generateNonDeltaComponentSample, evaluate_pdf_func evaluatePdf)
		: deltaComponentCount_(0)
		, hasNonDeltaComponents_(generateNonDeltaComponentSample != nullptr)
		, generateNonDeltaComponentSample_(generateNonDeltaComponentSample)
		, evaluatePdf_(std::move(evaluatePdf))
		, generateSample_(std::move(generateNonDeltaComponentSample))
	{
	}

	FunctionalDistribution(
		std::size_t deltaComponentCount,
		generate_sample_func generateDeltaComponentSample)
		: deltaComponentCount_(deltaComponentCount)
		, hasNonDeltaComponents_(false)
		, generateDeltaComponentSample_(generateDeltaComponentSample)
		, generateNonDeltaComponentSample_(nullptr)
		, evaluatePdf_(zero_evaluate_pdf)
		, generateSample_(std::move(generateDeltaComponentSample))
	{
	}


	std::size_t delta_components() const override
	{
		return deltaComponentCount_;
	}

	math::random_sample<TSample, TProbability> generate_delta_component_sample() const override
	{
		return generateDeltaComponentSample_();
	}

	bool has_non_delta_component() const override
	{
		return hasNonDeltaComponents_;
	}

	math::random_sample<TSample, TProbability> generate_non_delta_component_sample() const override
	{
		return generateNonDeltaComponentSample_();
	}

	TProbability evaluate_pdf(const TPdfSample& sample) const override
	{
		return evaluatePdf_(sample);
	}

	math::random_sample<TSample, TProbability> generate_sample() const override
	{
		return generateSample_();
	}

private:
	static TProbability zero_evaluate_pdf(const TPdfSample& sample)
	{
		return TProbability(0);
	}
};
