#include "TransformCommandFactory.h"

#include "TransformCommandProcessor.h"

std::vector<std::string> TransformCommandFactory::GetSupportedCommands() const
{
	return
	{
		"translate",
		"scale",
		"rotate",
		"pushTransform",
		"popTransform"
	};
}

CommandProcessor * TransformCommandFactory::CreateCommandProcessor(const std::experimental::filesystem::path& sceneFileName) const
{
	return new TransformCommandProcessor();
}
