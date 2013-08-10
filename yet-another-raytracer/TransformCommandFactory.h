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

	virtual std::vector<std::string> GetSupportedCommands() const;

	virtual CommandProcessor * CreateCommandProcessor() const;

};

