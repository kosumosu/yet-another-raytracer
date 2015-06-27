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
		auto position = ParserHelper::ReadVec3(stream);
		auto target = ParserHelper::ReadVec3(stream);
		auto up = ParserHelper::ReadVec3(stream);
		space_real fovy = ParserHelper::ReadSpaceReal(stream);

		auto binormal = math::cross(target - position, up);
		auto corrected_up = math::normalize(math::cross(binormal, target - position));

		context.scene()->camera()->position(position);
		context.scene()->camera()->target(target);
		context.scene()->camera()->up(corrected_up);
		context.scene()->camera()->fovy(fovy);
	}
	else
	{
		throw std::exception();
	}
}
