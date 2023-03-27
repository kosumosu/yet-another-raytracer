#pragma once
#include <type_traits>
#include <memory>

namespace math
{
	template <typename TValue>
	class UniformRandomBitGenerator
	{
	public:
		virtual ~UniformRandomBitGenerator() = default;

		using result_type = TValue;

		[[nodiscard]] virtual result_type min() const = 0;
		[[nodiscard]] virtual result_type max() const = 0;
		[[nodiscard]] virtual result_type operator()() const = 0;
	};

	template <typename TValue, typename TEngine/*, class = typename std::enable_if_t<std::is_same<TValue, typename TEngine::result_type>::value>*/>
	class StdUniformRandomBitGenerator : public UniformRandomBitGenerator<TValue>
	{
		static_assert(std::is_same<TValue, typename TEngine::result_type>::value, "Engine's result_type must be TValue");
	public:
		explicit StdUniformRandomBitGenerator(TEngine engine)
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

		typename UniformRandomBitGenerator<TValue>::result_type operator()() const override
		{
			return _engine();
		}


	private:
		mutable TEngine _engine;
	};
}
