#pragma once

#include "CommandFactory.h"

class ObjectCommandFactory : public CommandFactory
{
public:
	ObjectCommandFactory(void);
	virtual ~ObjectCommandFactory(void);

	std::vector<std::string> GetSupportedCommands() const override;

	CommandProcessor * CreateCommandProcessor(const std::experimental::filesystem::path& sceneFileName) const override;

};

