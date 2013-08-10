#pragma once

#include "CommandProcessor.h"

class SettingsCommandProcessor : public CommandProcessor
{
public:
	SettingsCommandProcessor(void);
	virtual ~SettingsCommandProcessor(void);

	virtual void PrepareContext( LoadingContext & context );

	virtual void ProcessCommand( LoadingContext & context, const std::string & command, std::istream & stream );

};

