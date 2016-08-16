#include "LightSourceCommandFactory.h"

#include "LightSourceCommandProcessor.h"

LightSourceCommandFactory::LightSourceCommandFactory(void)
{
}


LightSourceCommandFactory::~LightSourceCommandFactory(void)
{
}

std::vector<std::string> LightSourceCommandFactory::GetSupportedCommands() const
{
	std::vector<std::string> commands;

	commands.push_back("directional");
	commands.push_back("point");
	commands.push_back("attenuation");
	commands.push_back("geometryLight");

	return commands;
}

CommandProcessor * LightSourceCommandFactory::CreateCommandProcessor() const
{
	return new LightSourceCommandProcessor();
}
