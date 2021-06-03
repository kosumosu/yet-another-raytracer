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
	return 
	{
		"size",
		"maxdepth",
		"output",
		"background",
		"samples",
		"crop"
	};
}

CommandProcessor * SettingsCommandFactory::CreateCommandProcessor(const std::filesystem::path& sceneFileName) const
{
	return new SettingsCommandProcessor();
}