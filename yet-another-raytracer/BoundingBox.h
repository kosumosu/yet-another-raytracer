#pragma once

#include "BoundingBoxHit.h"
#include "Ray.h"
#include <algorithm>

class __declspec(align(MM_ALIGNMENT)) BoundingBox
{
public:

	BoundingBox(void)
		: m_min_corner(vector3())
		, m_max_corner(vector3(space_real(-1.0)))
	{
	}

	BoundingBox(const vector3 & min_corner, const vector3 & max_corner)
		: m_min_corner(min_corner)
		, m_max_corner(max_corner)
	{

	}

	~BoundingBox(void)
	{
	}

	bool empty() const
	{
		return m_min_corner.x() > m_max_corner.x() || m_min_corner.y() > m_max_corner.y() || m_min_corner.z() > m_max_corner.z();
	}

	const vector3 & min_corner() const { return m_min_corner; }
	void min_corner(const vector3 & val) { m_min_corner = val; }

	const vector3 & max_corner() const { return m_max_corner; }
	void max_corner(const vector3 & val) { m_max_corner = val; }


	BoundingBoxHit IntersectsRay(const Ray & ray) const
	{
		auto dif_min = m_min_corner - ray.origin();
		auto dif_max = m_max_corner - ray.origin();

		auto dist0 = ray.inversed_direction() * dif_min;
		auto dist1 = ray.inversed_direction() * dif_max;

		auto dist_min = math::min(dist0, dist1);
		auto dist_max = math::max(dist0, dist1);

		space_real dist_near = std::max(dist_min.x(), std::max(dist_min.y(), dist_min.z()));
		space_real dist_far = std::min(dist_max.x(), std::min(dist_max.y(), dist_max.z()));

		return BoundingBoxHit(dist_near, dist_far, dist_far > space_real(0.0) && dist_near <= dist_far);
	}

	void Include(const BoundingBox & other)
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

	void Include(const vector3 & point)
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

	bool Contains(const vector3 & point) const
	{
		return !empty() && point.x() >= m_min_corner.x() && point.y() >= m_min_corner.y() && point.z() >= m_min_corner.z() &&
			point.x() <= m_max_corner.x() && point.y() <= m_max_corner.y() && point.z() <= m_max_corner.z();
	}

	void Intersect(const BoundingBox & other)
	{
		m_min_corner = math::max(m_min_corner, other.min_corner());
		m_max_corner = math::min(m_max_corner, other.max_corner());
	}

	bool Intersects(const BoundingBox & other) const
	{
		BoundingBox temp = *this;
		temp.Intersect(other);
		return !temp.empty();
	}

	bool FullyInside(const BoundingBox & container) const
	{
		return m_min_corner.x() >= container.min_corner().x() && m_min_corner.y() >= container.min_corner().y() && m_min_corner.z() >= container.min_corner().z()
			&& m_max_corner.x() <= container.max_corner().x() && m_max_corner.y() <= container.max_corner().y() && m_max_corner.z() <= container.max_corner().z();
	}

	void Split(unsigned int axis, space_real coord, BoundingBox & box_left, BoundingBox & box_right) const
	{
		vector3 max_corner_left = m_max_corner;
		vector3 min_corner_right = m_min_corner;

		max_corner_left[axis] = coord;
		min_corner_right[axis] = coord;

		box_left.min_corner(m_min_corner);
		box_left.max_corner(max_corner_left);

		box_right.min_corner(min_corner_right);
		box_right.max_corner(m_max_corner);
	}

	void ExtendForEpsilon()
	{
		const space_real minimum_positive = std::numeric_limits<space_real>::min();
		m_min_corner -= math::max(math::abs(m_min_corner) * std::numeric_limits<space_real>::epsilon(), vector3(minimum_positive));
		m_max_corner += math::max(math::abs(m_max_corner) * std::numeric_limits<space_real>::epsilon(), vector3(minimum_positive));
	}

private:
	vector3 m_min_corner;
	vector3 m_max_corner;
};

