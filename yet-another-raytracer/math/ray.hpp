#pragma once

#include "vector.hpp"
#include "vector_functions.hpp"

namespace math
{
	template <typename TSpace, std::size_t DIMENSIONS>
	class ray
	{
	public:
		ray()
			: m_origin(vector<TSpace, DIMENSIONS>::zero())
			, m_direction(vector<TSpace, DIMENSIONS>::zero())
			, m_inversed_direction(vector<TSpace, DIMENSIONS>::zero())
		{ }

		ray(const vector<TSpace, DIMENSIONS> & origin, const vector<TSpace, DIMENSIONS> & direction)
			: m_origin(origin)
			  , m_direction(direction)
			  , m_inversed_direction(reciprocal(direction)) { }

		const vector<TSpace, DIMENSIONS> & origin() const
		{
			return m_origin;
		}

		void origin(const vector<TSpace, DIMENSIONS> & val)
		{
			m_origin = val;
		}

		const vector<TSpace, DIMENSIONS> & direction() const
		{
			return m_direction;
		}

		void direction(const vector<TSpace, DIMENSIONS> & val)
		{
			m_direction = val;
			m_inversed_direction = reciprocal(val);
		}

		const vector<TSpace, DIMENSIONS> & inversed_direction() const
		{
			return m_inversed_direction;
		}

		void inversed_direction(const vector<TSpace, DIMENSIONS> & val)
		{
			m_inversed_direction = val;
		}

		vector<TSpace, DIMENSIONS> point_along(const TSpace& distance) const
		{
			return math::fma(distance, m_direction, m_origin);
		}

		ray<TSpace, DIMENSIONS> ray_along(const TSpace& distance) const
		{
			return {point_along(distance), m_direction};
		}
	
	private:
		vector<TSpace, DIMENSIONS> m_origin;
		vector<TSpace, DIMENSIONS> m_direction;
		vector<TSpace, DIMENSIONS> m_inversed_direction;

		ray(const vector<TSpace, DIMENSIONS> & origin, const vector<TSpace, DIMENSIONS> & direction, const vector<TSpace, DIMENSIONS> & inversedDirection)
			: m_origin(origin)
			, m_direction(direction)
			, m_inversed_direction(inversedDirection) { }
	};
}
