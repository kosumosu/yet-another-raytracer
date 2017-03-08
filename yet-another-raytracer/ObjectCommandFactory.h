#pragma once

#include "CommandFactory.h"

class ObjectCommandFactory : public CommandFactory
{
public:
	ObjectCommandFactory(void);
	virtual ~ObjectCommandFactory(void);

	std::vector<std::string> GetSupportedCommands() const override;

	CommandProcessor * CreateCommandProcessor(const std::wstring & sceneFileName) const override;

};

