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
	std::vector<std::string> commands;

	commands.push_back("camera");

	return commands;
}

CommandProcessor * CameraCommandFactory::CreateCommandProcessor() const
{
	return new CameraCommandProcessor();
}
