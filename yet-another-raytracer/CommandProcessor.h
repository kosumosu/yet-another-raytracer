#pragma once

#include "LoadingContext.h"
#include <string>
#include <map>
#include <memory>
#include <istream>

class CommandProcessor
{
public:

	CommandProcessor(void)
	{
	}

	virtual ~CommandProcessor(void)
	{
	}

	virtual void PrepareContext(LoadingContext & context) = 0;

	virtual void ProcessCommand(LoadingContext & context, const std::string & command, std::istream & stream) = 0;
};

typedef std::map< std::string, std::shared_ptr<CommandProcessor> > ProcessorMap;
typedef std::vector< std::shared_ptr<CommandProcessor> > ProcessorCollection;