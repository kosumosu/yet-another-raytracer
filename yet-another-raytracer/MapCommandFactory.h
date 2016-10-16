#pragma once
#include "CommandFactory.h"

class MapCommandFactory : public CommandFactory
{
public:
	std::vector<std::string> GetSupportedCommands() const override;
	CommandProcessor * CreateCommandProcessor() const override;
};
