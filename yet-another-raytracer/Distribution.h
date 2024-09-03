#pragma once

#include <functional>
#include <utility>
#include "math/random_sample.hpp"

template <typename TSample, typename TPdfSample, typename TProbability>
class Distribution
{
public:
	using delta_func = std::function<void(const math::random_sample<TSample, TProbability>& sample)>;

	virtual ~Distribution() = default;

	virtual std::size_t delta_components() const = 0;
	virtual bool has_non_delta_component() const = 0;
	virtual math::random_sample<TSample, TProbability> generate_sample() const = 0;
};


template <typename TSample, typename TPdfSample, typename TProbability>
class FunctionalDistribution final : public Distribution<TSample, TPdfSample, TProbability>
{
public:
	using delta_func = std::function<void(const math::random_sample<TSample, TProbability>& sample)>;

	using generate_sample_func = std::function<math::random_sample<TSample, TProbability>()>;
	using evaluate_pdf_func = std::function<TProbability(const TPdfSample& sample)>;

private:
	const std::size_t deltaComponentCount_;
	const bool hasNonDeltaComponents_;
	const generate_sample_func generateSample_;

public:

	FunctionalDistribution()
		: deltaComponentCount_(0)
		, hasNonDeltaComponents_(false)
		, generateSample_(nullptr)
	{
	}

	FunctionalDistribution(
		std::size_t deltaComponentCount,
		bool hasNonDeltaComponents,
		generate_sample_func generateSample)
		: deltaComponentCount_(deltaComponentCount)
		, hasNonDeltaComponents_(hasNonDeltaComponents)
		, generateSample_(std::move(generateSample))
	{
	}

	std::size_t delta_components() const override
	{
		return deltaComponentCount_;
	}


	bool has_non_delta_component() const override
	{
		return hasNonDeltaComponents_;
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

template <typename TSample, typename TPdfSample, typename TProbability>
class TrivialDistribution final : public Distribution<TSample, TPdfSample, TProbability>
{
	TSample value_;
public:
	explicit TrivialDistribution(TSample value)
		: value_(std::move(value))
	{
	}

	std::size_t delta_components() const override { return 1; }

	bool has_non_delta_component() const override { return false; }

	math::random_sample<TSample, TProbability> generate_sample() const override
	{
		return { value_, TProbability(1.0), true };
	}
};