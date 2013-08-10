#include "TransformCommandFactory.h"

#include "TransformCommandProcessor.h"

CommandProcessor * TransformCommandFactory::CreateCommandProcessor() const
{
	return new TransformCommandProcessor();
}

std::vector<std::string> TransformCommandFactory::GetSupportedCommands() const
{
	std::vector<std::string> commands;
	commands.push_back("translate");
	commands.push_back("scale");
	commands.push_back("rotate");
	commands.push_back("pushTransform");
	commands.push_back("popTransform");
	return commands;
}
