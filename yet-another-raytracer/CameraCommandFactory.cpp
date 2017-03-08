#include "CameraCommandFactory.h"

#include "CameraCommandProcessor.h"

CameraCommandFactory::CameraCommandFactory(void)
{
}


CameraCommandFactory::~CameraCommandFactory(void)
{
}

std::vector<std::string> CameraCommandFactory::GetSupportedCommands() const
{
	return{ "camera" };
}

CommandProcessor * CameraCommandFactory::CreateCommandProcessor(const std::wstring & sceneFileName) const
{
	return new CameraCommandProcessor();
}
