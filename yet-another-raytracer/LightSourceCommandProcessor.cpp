#include "LightSourceCommandProcessor.h"

#include "DirectionalLightSource.h"
#include "PointLightSource.h"
#include "GeometryLightSource.h"
#include "ParserHelper.h"
#include "Types.h"


LightSourceCommandProcessor::LightSourceCommandProcessor(void)
	: m_attenuation(1.0f, 0.0f, 0.0f)
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
		const auto direction = ParserHelper::ReadVec3(stream);
		const auto color = ParserHelper::ReadColorRgb(stream);

		DirectionalLightSource * light = new DirectionalLightSource(
			(context.transform() * vector4(direction, space_real(0.0))).reduce(),
			color,
			*context.scene()
			);

		context.scene()->lights().push_back(std::shared_ptr<LightSource>(light));
	}
	else if (command == "point")
	{
		const auto position = ParserHelper::ReadVec3(stream);
		const auto color = ParserHelper::ReadColorRgb(stream);

        auto light = std::make_shared<PointLightSource>();
        light->position((context.transform() * vector4(position, space_real(1.0))).reduce());
        light->color(color);
        light->attenuation(m_attenuation);

        context.scene()->lights().push_back(std::move(light));
    } else if (command == "attenuation") {
        space_real constant, linear, quadratic;
        stream >> constant >> linear >> quadratic;

		m_attenuation.constant(constant);
		m_attenuation.linear(linear);
		m_attenuation.quadratic(quadratic);
	}
	else if (command == "geometryLight")
	{
		std::shared_ptr<GeometryLightSource> light(new GeometryLightSource(context.scene()->objects()));

		context.scene()->lights().push_back(std::move(light));
	}
	else
	{
		throw std::exception();
	}
}
