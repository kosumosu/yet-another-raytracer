#pragma once

#include "CommandFactory.h"

class TransformCommandFactory : public CommandFactory
{
public:

	TransformCommandFactory(void)
	{
	}

	virtual ~TransformCommandFactory(void)
	{
	}

	std::vector<std::string> GetSupportedCommands() const override;

	CommandProcessor * CreateCommandProcessor(const std::wstring & sceneFileName) const override;

};

