#pragma once

#include "LoadingContext.h"
#include <string>
#include <map>
#include <memory>
#include <istream>

class CommandProcessor
{
public:
	virtual ~CommandProcessor() = default;
	virtual void PrepareContext(LoadingContext & context) = 0;
	virtual void ProcessCommand(LoadingContext & context, const std::string & command, std::istream & stream) = 0;
};

using ProcessorMap = std::map<std::string, std::shared_ptr<CommandProcessor>>;
using ProcessorCollection = std::vector<std::shared_ptr<CommandProcessor>>;
