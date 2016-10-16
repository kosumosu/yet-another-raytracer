#include "MapCommandFactory.h"
#include "MapCommandProcessor.h"

std::vector<std::string> MapCommandFactory::GetSupportedCommands() const
{
	return {"solidColor", "visualizeUV", "nullMap"};
}

CommandProcessor * MapCommandFactory::CreateCommandProcessor() const
{
	return new MapCommandProcessor();
}
