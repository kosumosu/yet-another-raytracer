#include "MaterialCommandFactory.h"

#include "MaterialCommandProcessor.h"

MaterialCommandFactory::MaterialCommandFactory(void)
{
}


MaterialCommandFactory::~MaterialCommandFactory(void)
{
}

std::vector<std::string> MaterialCommandFactory::GetSupportedCommands() const
{
	std::vector<std::string> commands;

	commands.push_back("blinn");
	commands.push_back("ambient");
	commands.push_back("emission");
	commands.push_back("diffuse");
	commands.push_back("specular");
	commands.push_back("shininess");

	commands.push_back("dielectric");
	commands.push_back("iorInside");
	commands.push_back("iorOutside");

	return commands;
}

CommandProcessor * MaterialCommandFactory::CreateCommandProcessor() const
{
	return new MaterialCommandProcessor();
}
