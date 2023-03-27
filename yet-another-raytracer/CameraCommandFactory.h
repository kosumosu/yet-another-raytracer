#pragma once

#include "CommandFactory.h"

class CameraCommandFactory : public CommandFactory
{
public:
	CameraCommandFactory(void);
	virtual ~CameraCommandFactory(void);

	std::vector<std::string> GetSupportedCommands() const override;

	CommandProcessor * CreateCommandProcessor(const std::experimental::filesystem::path& sceneFileName) const override;

};

