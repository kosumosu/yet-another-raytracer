#pragma once

#include "CommandFactory.h"

class SettingsCommandFactory : public CommandFactory
{
public:
	SettingsCommandFactory(void);
	~SettingsCommandFactory(void);

	std::vector<std::string> GetSupportedCommands() const override;

	CommandProcessor * CreateCommandProcessor() const override;
};

