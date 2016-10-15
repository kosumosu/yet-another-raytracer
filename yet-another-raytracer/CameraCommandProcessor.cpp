#include "CameraCommandProcessor.h"

#include "ParserHelper.h"
#include "Camera.h"

CameraCommandProcessor::CameraCommandProcessor(void)
{
}


CameraCommandProcessor::~CameraCommandProcessor(void)
{
}

void CameraCommandProcessor::PrepareContext( LoadingContext & context )
{
	// do nothing
}

void CameraCommandProcessor::ProcessCommand( LoadingContext & context, const std::string & command, std::istream & stream )
{
	if (command == "camera")
	{
		const auto position = ParserHelper::ReadVec3(stream);
		const auto target = ParserHelper::ReadVec3(stream);
		const auto up = ParserHelper::ReadVec3(stream);
		const space_real fovy = ParserHelper::ReadSpaceReal(stream);

		const auto transformed_up = (context.transform() * vector4(up, 0.0f)).reduce();

		const auto binormal = math::cross(target - position, transformed_up);
		const auto corrected_up = math::normalize(math::cross(binormal, target - position));

		context.scene()->camera()->position((context.transform() * vector4(position, 1.0f)).reduce());
		context.scene()->camera()->target((context.transform() * vector4(target, 1.0f)).reduce());
		context.scene()->camera()->up((context.transform() * vector4(corrected_up, 0.0f)).reduce());
		context.scene()->camera()->fovy(fovy);
	}
	else
	{
		throw std::exception();
	}
}
