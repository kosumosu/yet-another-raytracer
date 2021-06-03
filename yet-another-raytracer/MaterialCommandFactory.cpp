#include "MaterialCommandFactory.h"

#include "MaterialCommandProcessor.h"

MaterialCommandFactory::MaterialCommandFactory(void) {}


MaterialCommandFactory::~MaterialCommandFactory(void) {}

std::vector<std::string> MaterialCommandFactory::GetSupportedCommands() const
{
	return
	{
		"selectMaterial",

		"blinn",
		"ambient",
		"emission",
		"diffuseMap",
		"diffuse",
		"specular",
		"shininess",
		"translucency",

		"dielectric",
		"iorInside",
		"iorOutside",
		"surfaceTransparency"
	};
}

CommandProcessor * MaterialCommandFactory::CreateCommandProcessor(const std::filesystem::path& sceneFileName) const
{
	return new MaterialCommandProcessor();
}
