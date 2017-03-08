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

CommandProcessor * TransformCommandFactory::CreateCommandProcessor(const std::wstring & sceneFileName) const
{
	return new TransformCommandProcessor();
}
