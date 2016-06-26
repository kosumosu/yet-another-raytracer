#pragma once

#include "Ray.h"

#include "Types.h"


class Camera
{
public:
	
	Camera(void)
	: m_position(vector3(1.0, 0.0, 0.0))
	, m_target(vector3(0.0, 0.0, 0.0))
	{
		UpdateDirection();
	}

	~Camera(void)
	{
	}


	const vector3 & position() const { return m_position; }
	void position(const vector3 & value)
	{
		m_position = value;
		UpdateDirection();
	}

	const vector3 & up() const { return m_up; }
	void up(const vector3 & value) { m_up = value; }

	const vector3 & target() const { return m_target; }
	void target(const vector3 & value)
	{
		m_target = value;
		UpdateDirection();
	}

	space_real fovy() const { return m_fovy; }
	void fovy(space_real value)
	{
		m_fovy = value;
		m_tan_half_fov_y = std::tan(space_real(math::pi) * m_fovy / space_real(360.0));
	}

	Ray GetViewRay(vector2 imageSpaceCoord, space_real aspect) const
	{
		auto w = m_direction;
		auto v = m_up;
		auto u = math::cross(w, v);

		auto scale_y = m_tan_half_fov_y;
		auto scale_x = m_tan_half_fov_y * aspect;

		vector3 ray_direction = w + u * (MapToCamera(imageSpaceCoord[0]) * scale_x) - v * (MapToCamera(imageSpaceCoord[1]) * scale_y);  // -v because of inverted y

		return Ray(m_position, math::normalize(ray_direction));
	}

private:
	vector3 m_position;
	vector3 m_up;
	vector3 m_target;
	vector3 m_direction;

	space_real m_fovy;
	space_real m_tan_half_fov_y;

	void UpdateDirection() 
	{
		m_direction = math::normalize(m_target - m_position);
	}

	space_real MapToCamera(space_real value) const
	{
		return (value - 0.5f) * 2.0f;
	}
};

