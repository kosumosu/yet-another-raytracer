#pragma once

#include "CommandFactory.h"

class MaterialCommandFactory : public CommandFactory
{
public:
	MaterialCommandFactory(void);
	~MaterialCommandFactory(void);

	std::vector<std::string> GetSupportedCommands() const override;

	CommandProcessor * CreateCommandProcessor() const override;

};

