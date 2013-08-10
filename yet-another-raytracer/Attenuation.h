#pragma once

#include "Types.h"

class Attenuation
{
public:

	Attenuation()
	{

	}

	Attenuation(space_real quadratic, space_real linear, space_real constant)
		: m_quadratic(quadratic)
		, m_linear(linear)
		, m_constant(constant)
	{
	}

	~Attenuation(void)
	{
	}

	space_real quadratic() const { return m_quadratic; }
	void quadratic(space_real val) { m_quadratic = val; }

	space_real linear() const { return m_linear; }
	void linear(space_real val) { m_linear = val; }

	space_real constant() const { return m_constant; }
	void constant(space_real val) { m_constant = val; }

	color_real Evaluate(space_real distance) const
	{
		return (color_real)(space_real(1.0) / ( m_quadratic * distance * distance + m_linear * distance + m_constant ));
	}

private:
	space_real m_quadratic;
	space_real m_linear;
	space_real m_constant;
};

