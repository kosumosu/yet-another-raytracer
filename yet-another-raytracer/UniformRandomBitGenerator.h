#pragma once

namespace math
{
	template <typename TValue>
	class UniformRandomBitGenerator
	{
	protected:
		UniformRandomBitGenerator()
		{
		}
	public:
		virtual ~UniformRandomBitGenerator() { }

		typedef TValue result_type;

		virtual result_type min() const = 0;
		virtual result_type max() const = 0;
		virtual result_type operator()() = 0;
	};

	template <typename TValue, typename TEngine, class = typename std::enable_if<std::is_same<TValue, typename TEngine::result_type>::value>::type>
	class StdUniformRandomBitGenerator : public UniformRandomBitGenerator<TValue>
	{
	public:
		explicit StdUniformRandomBitGenerator(const TEngine & engine)
			: _engine(engine)
		{
		}

		explicit StdUniformRandomBitGenerator(TEngine && engine)
			: _engine(std::move(engine))
		{
		}

		typename UniformRandomBitGenerator<TValue>::result_type min() const override
		{
			return _engine.min();
		}

		typename UniformRandomBitGenerator<TValue>::result_type max() const override
		{
			return _engine.max();
		}

		typename UniformRandomBitGenerator<TValue>::result_type operator()() override
		{
			return _engine();
		}
	private:
		TEngine _engine;
	};
}