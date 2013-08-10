#include "SettingsCommandFactory.h"

#include "SettingsCommandProcessor.h"


SettingsCommandFactory::SettingsCommandFactory( void )
{

}

SettingsCommandFactory::~SettingsCommandFactory( void )
{

}


std::vector<std::string> SettingsCommandFactory::GetSupportedCommands() const
{
	std::vector<std::string> commands;
	commands.push_back("size");
	commands.push_back("maxdepth");
	commands.push_back("output");
	return commands;
}

CommandProcessor * SettingsCommandFactory::CreateCommandProcessor() const
{
	return new SettingsCommandProcessor();
}