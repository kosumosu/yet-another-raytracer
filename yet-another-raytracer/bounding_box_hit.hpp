#pragma once

namespace math
{
	template <typename TSpace>
	class bounding_box_hit
	{
	public:

		bounding_box_hit()
			: m_near(0)
			  , m_far(0)
			  , m_occurred(false) { }

		bounding_box_hit(const TSpace & near, const TSpace & far)
			: m_near(near)
			  , m_far(far)
			  , m_occurred(true) { }

		bounding_box_hit(const TSpace & near, const TSpace & far, bool occured)
			: m_near(near)
			  , m_far(far)
			  , m_occurred(occured) { }

		~bounding_box_hit(void) { }

		TSpace near() const
		{
			return m_near;
		}

		TSpace far() const
		{
			return m_far;
		}

		bool occurred() const
		{
			return m_occurred;
		}

		bool is_inside_box() const
		{
			return m_near < TSpace(0.0) && m_far >= TSpace(0.0);
		}

	private:
		TSpace m_near;
		TSpace m_far;
		bool m_occurred;
	};
}
