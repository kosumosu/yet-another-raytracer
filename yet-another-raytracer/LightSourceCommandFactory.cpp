#include "LightSourceCommandFactory.h"

#include "LightSourceCommandProcessor.h"

LightSourceCommandFactory::LightSourceCommandFactory(void) {}


LightSourceCommandFactory::~LightSourceCommandFactory(void) {}

std::vector<std::string> LightSourceCommandFactory::GetSupportedCommands() const
{
	return {"directional", "point", "attenuation", "geometryLight"};
}

CommandProcessor * LightSourceCommandFactory::CreateCommandProcessor(const std::wstring & sceneFileName) const
{
	return new LightSourceCommandProcessor();
}
