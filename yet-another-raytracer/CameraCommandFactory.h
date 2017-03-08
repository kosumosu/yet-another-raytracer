#pragma once

#include "CommandFactory.h"

class CameraCommandFactory : public CommandFactory
{
public:
	CameraCommandFactory(void);
	virtual ~CameraCommandFactory(void);

	std::vector<std::string> GetSupportedCommands() const override;

	CommandProcessor * CreateCommandProcessor(const std::wstring & sceneFileName) const override;

};

