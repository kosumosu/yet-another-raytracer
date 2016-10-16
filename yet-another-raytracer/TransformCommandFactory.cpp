#include "TransformCommandFactory.h"

#include "TransformCommandProcessor.h"

CommandProcessor * TransformCommandFactory::CreateCommandProcessor() const
{
	return new TransformCommandProcessor();
}

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
