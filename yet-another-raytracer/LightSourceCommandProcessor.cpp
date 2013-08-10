#include "LightSourceCommandProcessor.h"

#include "DirectionalLightSource.h"
#include "PointLightSource.h"
#include "PareserHelper.h"
#include "Types.h"

LightSourceCommandProcessor::LightSourceCommandProcessor(void)
	: m_attenuation(0.0f, 0.0f, 1.0f)
{
}


LightSourceCommandProcessor::~LightSourceCommandProcessor(void)
{
}

void LightSourceCommandProcessor::PrepareContext( LoadingContext & context )
{
	// do nothing
}

void LightSourceCommandProcessor::ProcessCommand( LoadingContext & context, const std::string & command, std::istream & stream )
{
	if (command == "directional")
	{
		auto direction = PareserHelper::ReadVec3(stream);
		auto color = color4(PareserHelper::ReadVec3(stream), color_real(0.0));

		DirectionalLightSource * light = new DirectionalLightSource();
		light->direction((context.transform() * vector4(direction, space_real(0.0))).reduce());
		light->color(color);

		context.scene()->lights()->push_back(std::shared_ptr<LightSource>(light));
	}
	else if (command == "point")
	{
		auto position = PareserHelper::ReadVec3(stream);
		auto color = color4(PareserHelper::ReadVec3(stream), 0.0f);

		PointLightSource * light = new PointLightSource();
		light->position((context.transform() * vector4(position, space_real(1.0))).reduce());
		light->color(color);
		light->attenuation(m_attenuation);

		context.scene()->lights()->push_back(std::shared_ptr<LightSource>(light));
	}
	else if (command == "attenuation")
	{
		space_real constant, linear, quadratic;
		stream >> constant >> linear >> quadratic;

		m_attenuation.constant(constant);
		m_attenuation.linear(linear);
		m_attenuation.quadratic(quadratic);
	}
	else
	{
		throw std::exception();
	}
}
