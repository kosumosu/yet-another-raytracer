#pragma once

#include "CommandFactory.h"

class SettingsCommandFactory : public CommandFactory
{
public:
	SettingsCommandFactory(void);
	~SettingsCommandFactory(void);

	virtual std::vector<std::string> GetSupportedCommands() const;

	virtual CommandProcessor * CreateCommandProcessor() const;
};

