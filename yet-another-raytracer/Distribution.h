#pragma once

#include <functional>
#include "random_sample.hpp"

template <typename TSample, typename TPdfSample, typename TProbability>
class Distribution
{
public:
	using delta_func = std::function<void(const math::random_sample<TSample, TProbability> & sample)>;

	virtual ~Distribution() = default;

	virtual std::size_t delta_components() const = 0;
	virtual void iterate_over_delta_components(const delta_func & job) const = 0;
	virtual math::random_sample<TSample, TProbability> generate_delta_component_sample() const = 0;
	virtual bool has_non_delta_component() const = 0;
	virtual math::random_sample<TSample, TProbability> generate_non_delta_component_sample() const = 0;
	virtual TProbability evaluate_pdf(const TPdfSample & sample) const = 0;
	virtual math::random_sample<TSample, TProbability> generate_sample() const = 0;
};


template <typename TSample, typename TPdfSample, typename TProbability>
class FunctionalDistribution : public Distribution<TSample, TPdfSample, TProbability>
{
public:
	using delta_func = std::function<void(const math::random_sample<TSample, TProbability> & sample)>;

	using iterate_over_delta_func = std::function<void(const delta_func & job)>;
	using generate_sample_func = std::function<math::random_sample<TSample, TProbability>()>;
	using evaluate_pdf_func = std::function<TProbability(const TPdfSample & sample)>;

	FunctionalDistribution()
		: _deltaComponentCount(0),
		  _hasNonDeltaComponents(false),
		  _iterateOverDeltaComponents(nullptr),
		  _generateNonDeltaComponentSample(nullptr),
		  _evaluatePdf(zero_evaluate_pdf),
		  _generateSample(nullptr) { }

	FunctionalDistribution(
		std::size_t deltaComponentCount,
		const iterate_over_delta_func & iterateOverDeltaComponents,
		const generate_sample_func & generateDeltaComponentSample,
		const generate_sample_func & generateNonDeltaComponentSample,
		const evaluate_pdf_func & evaluatePdf,
		const generate_sample_func & generateSample
	)
		: _deltaComponentCount(deltaComponentCount),
		  _hasNonDeltaComponents(generateNonDeltaComponentSample != nullptr),
		  _iterateOverDeltaComponents(iterateOverDeltaComponents),
		  _generateDeltaComponentSample(generateDeltaComponentSample),
		  _generateNonDeltaComponentSample(generateNonDeltaComponentSample),
		  _evaluatePdf(evaluatePdf),
		  _generateSample(generateSample) { }

	explicit FunctionalDistribution(
		const generate_sample_func & generateNonDeltaComponentSample,
		const evaluate_pdf_func & evaluatePdf
	)
		: _deltaComponentCount(0),
		  _hasNonDeltaComponents(generateNonDeltaComponentSample != nullptr),
		  _iterateOverDeltaComponents(nullptr),
		  _generateNonDeltaComponentSample(generateNonDeltaComponentSample),
		  _evaluatePdf(evaluatePdf),
		  _generateSample(generateNonDeltaComponentSample) { }

	FunctionalDistribution(
		std::size_t deltaComponentCount,
		const iterate_over_delta_func & iterateOverDeltaComponents,
		const generate_sample_func & generateDeltaComponentSample
	)
		: _deltaComponentCount(deltaComponentCount),
		  _hasNonDeltaComponents(false),
		  _iterateOverDeltaComponents(iterateOverDeltaComponents),
		  _generateDeltaComponentSample(generateDeltaComponentSample),
		  _generateNonDeltaComponentSample(nullptr),
		  _evaluatePdf(zero_evaluate_pdf),
		  _generateSample(generateDeltaComponentSample) { }


	std::size_t delta_components() const override
	{
		return _deltaComponentCount;
	}

	void iterate_over_delta_components(const delta_func & job) const override
	{
		_iterateOverDeltaComponents(job);
	}

	math::random_sample<TSample, TProbability> generate_delta_component_sample() const override
	{
		return _generateDeltaComponentSample();
	}

	bool has_non_delta_component() const override
	{
		return _hasNonDeltaComponents;
	}

	math::random_sample<TSample, TProbability> generate_non_delta_component_sample() const override
	{
		return _generateNonDeltaComponentSample();
	}

	TProbability evaluate_pdf(const TPdfSample & sample) const override
	{
		return _evaluatePdf(sample);
	}

	math::random_sample<TSample, TProbability> generate_sample() const override
	{
		return _generateSample();
	}

private:
	std::size_t _deltaComponentCount;
	bool _hasNonDeltaComponents;
	iterate_over_delta_func _iterateOverDeltaComponents;
	generate_sample_func _generateDeltaComponentSample;
	generate_sample_func _generateNonDeltaComponentSample;
	evaluate_pdf_func _evaluatePdf;
	generate_sample_func _generateSample;

	static TProbability zero_evaluate_pdf(const TPdfSample & sample)
	{
		return TProbability(0);
	}
};
