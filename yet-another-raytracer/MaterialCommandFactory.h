#pragma once

#include "CommandFactory.h"

class MaterialCommandFactory : public CommandFactory
{
public:
	MaterialCommandFactory(void);
	~MaterialCommandFactory(void);

	virtual std::vector<std::string> GetSupportedCommands() const;

	virtual CommandProcessor * CreateCommandProcessor() const;

};

