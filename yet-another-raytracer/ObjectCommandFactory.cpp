#include "ObjectCommandFactory.h"

#include "ObjectCommandProcessor.h"

ObjectCommandFactory::ObjectCommandFactory(void)
{
}


ObjectCommandFactory::~ObjectCommandFactory(void)
{
}

std::vector<std::string> ObjectCommandFactory::GetSupportedCommands() const
{
	std::vector<std::string> commands;

	commands.push_back("maxverts");
	commands.push_back("maxvertnorms");
	commands.push_back("vertex");
	commands.push_back("vertexnormal");
	commands.push_back("tri");
	commands.push_back("trinormal");
	commands.push_back("sphere");

	return commands;
}

CommandProcessor * ObjectCommandFactory::CreateCommandProcessor() const
{
	return new ObjectCommandProcessor();
}
