#pragma once

#include "CommandFactory.h"

class LightSourceCommandFactory : public CommandFactory
{
public:
	LightSourceCommandFactory(void);
	virtual ~LightSourceCommandFactory(void);

	virtual std::vector<std::string> GetSupportedCommands() const;

	virtual CommandProcessor * CreateCommandProcessor() const;

};

