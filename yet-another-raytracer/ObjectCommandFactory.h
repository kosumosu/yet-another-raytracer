#pragma once

#include "CommandFactory.h"

class ObjectCommandFactory : public CommandFactory
{
public:
	ObjectCommandFactory(void);
	virtual ~ObjectCommandFactory(void);

	virtual std::vector<std::string> GetSupportedCommands() const;

	virtual CommandProcessor * CreateCommandProcessor() const;

};

