#pragma once

#include <string>
#include <vector>

class CommandProcessor;

class CommandFactory
{
public:
	virtual ~CommandFactory() = default;

	virtual std::vector<std::string> GetSupportedCommands() const = 0;

	virtual CommandProcessor * CreateCommandProcessor(const std::wstring & sceneFileName) const = 0;
};

