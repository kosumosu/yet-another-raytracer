#pragma once

#include "CommandFactory.h"

class LightSourceCommandFactory : public CommandFactory
{
public:
	LightSourceCommandFactory(void);
	virtual ~LightSourceCommandFactory(void);

	std::vector<std::string> GetSupportedCommands() const override;

	CommandProcessor * CreateCommandProcessor(const std::filesystem::path& sceneFileName) const override;

};

