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
	return 
	{
		"maxverts",
		"maxvertnorms",
		"vertex",
		"vertexnormal",
		"uv",
		"tri",
		"triUV",
		"trinormal",
		"sphere"
	};
}

CommandProcessor * ObjectCommandFactory::CreateCommandProcessor(const std::experimental::filesystem::path& sceneFileName) const
{
	return new ObjectCommandProcessor();
}
