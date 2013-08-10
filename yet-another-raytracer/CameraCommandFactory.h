#pragma once

#include "CommandFactory.h"

class CameraCommandFactory : public CommandFactory
{
public:
	CameraCommandFactory(void);
	virtual ~CameraCommandFactory(void);

	virtual std::vector<std::string> GetSupportedCommands() const;

	virtual CommandProcessor * CreateCommandProcessor() const;

};

