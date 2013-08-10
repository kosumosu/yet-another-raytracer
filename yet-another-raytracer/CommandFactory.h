#pragma once

#include "CommandProcessor.h"
#include <string>
#include <vector>
#include <iostream>

class CommandFactory
{
public:

	CommandFactory(void)
	{
	}

	virtual ~CommandFactory(void)
	{
	}

	virtual std::vector<std::string> GetSupportedCommands() const = 0;

	virtual CommandProcessor * CreateCommandProcessor() const = 0;
};

