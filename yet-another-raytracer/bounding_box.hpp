#pragma once

#include "bounding_box_hit.hpp"
#include "ray.hpp"
#include "vector_functions.hpp"
#include <algorithm>

namespace math
{
	template <typename TSpace, size_t DIMENSIONS>
	class bounding_box
	{
	public:

		bounding_box(void)
			: m_min_corner(vector<TSpace, DIMENSIONS>::zero())
			, m_max_corner(vector<TSpace, DIMENSIONS>::fill(-1))
		{
		}

		bounding_box(const math::ray<TSpace, DIMENSIONS> & min_corner, const math::ray<TSpace, DIMENSIONS> & max_corner)
			: m_min_corner(min_corner)
			, m_max_corner(max_corner)
		{

		}

		~bounding_box(void)
		{
		}

		bool empty() const
		{
			bool isEmpty = false;
			iterate<0, DIMENSIONS - 1>([&](int i)
			{
				isEmpty = isEmpty || m_min_corner[i] > m_max_corner[i];
			});
			return isEmpty;
		}

		const math::vector<TSpace, DIMENSIONS> & min_corner() const { return m_min_corner; }
		void min_corner(const math::vector<TSpace, DIMENSIONS> & val) { m_min_corner = val; }

		const math::vector<TSpace, DIMENSIONS> & max_corner() const { return m_max_corner; }
		void max_corner(const math::vector<TSpace, DIMENSIONS> & val) { m_max_corner = val; }


		void Include(const bounding_box & other)
		{
			if (empty())
			{
				m_min_corner = other.min_corner();
				m_max_corner = other.max_corner();
			}
			else if (!other.empty())
			{
				m_min_corner = math::min(m_min_corner, other.min_corner());
				m_max_corner = math::max(m_max_corner, other.max_corner());
			}
		}

		void Include(const vector<TSpace, DIMENSIONS> & point)
		{
			if (empty())
			{
				m_min_corner = point;
				m_max_corner = point;
			}
			else
			{
				m_min_corner = math::min(m_min_corner, point);
				m_max_corner = math::max(m_max_corner, point);
			}
		}

		bounding_box_hit<TSpace> IntersectsRay(const ray<TSpace, DIMENSIONS> & ray) const
		{
			auto dif_min = m_min_corner - ray.origin();
			auto dif_max = m_max_corner - ray.origin();

			auto dist0 = ray.inversed_direction() * dif_min;
			auto dist1 = ray.inversed_direction() * dif_max;

			auto dist_min = min(dist0, dist1);
			auto dist_max = max(dist0, dist1);

			auto dist_near = max_element(dist_min);
			auto dist_far = min_element(dist_max);

			return bounding_box_hit<TSpace>(dist_near, dist_far, dist_far > TSpace(0.0) && dist_near <= dist_far);
		}

		bool Contains(const vector<TSpace, DIMENSIONS> & point) const
		{
			bool isInside = !empty();
			iterate<0, DIMENSIONS - 1>([&](int i)
			{
				isInside = isInside && (m_min_corner[i] <= point[i] && m_max_corner[i] >= point[i]);
			});
			return isInside;
		}

		void Intersect(const bounding_box & other)
		{
			m_min_corner = math::max(m_min_corner, other.min_corner());
			m_max_corner = math::min(m_max_corner, other.max_corner());
		}

		bool Intersects(const bounding_box & other) const
		{
			auto minCorner = math::max(m_min_corner, other.min_corner());
			auto maxCorner = math::min(m_max_corner, other.max_corner());
			bounding_box<TSpace, DIMENSIONS> product(minCorner, maxCorner);
			return !product.empty();
		}

		bool FullyInside(const bounding_box & container) const
		{
			bool isInside = true;
			iterate<0, DIMENSIONS - 1>([&](int i)
			{
				isInside = isInside && (m_min_corner[i] >= container.min_corner()[i] && m_max_corner[i] <= container.max_corner()[i]);
			});
			return isInside;
		}

		void Split(size_t axis, const TSpace & coord, bounding_box & box_left, bounding_box & box_right) const
		{
			vector<TSpace, DIMENSIONS> max_corner_left = m_max_corner;
			vector<TSpace, DIMENSIONS> min_corner_right = m_min_corner;

			max_corner_left[axis] = coord;
			min_corner_right[axis] = coord;

			box_left.min_corner(m_min_corner);
			box_left.max_corner(max_corner_left);

			box_right.min_corner(min_corner_right);
			box_right.max_corner(m_max_corner);
		}

		void ExtendForEpsilon()
		{
			const TSpace minimum_positive = std::numeric_limits<TSpace>::min();
			m_min_corner -= math::max(math::abs(m_min_corner) * std::numeric_limits<TSpace>::epsilon(), vector<TSpace, DIMENSIONS>::fill(minimum_positive));
			m_max_corner += math::max(math::abs(m_max_corner) * std::numeric_limits<TSpace>::epsilon(), vector<TSpace, DIMENSIONS>::fill(minimum_positive));
		}

	private:
		vector<TSpace, DIMENSIONS> m_min_corner;
		vector<TSpace, DIMENSIONS> m_max_corner;
	};
}