#pragma once
#include "CommandFactory.h"

class TextureCommandFactory : public CommandFactory
{
public:
	std::vector<std::string> GetSupportedCommands() const override;
	CommandProcessor * CreateCommandProcessor(const std::wstring & sceneFileName) const override;
};
