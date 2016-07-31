#pragma once

#include "CommandProcessor.h"

class SettingsCommandProcessor : public CommandProcessor
{
public:
	SettingsCommandProcessor(void);
	virtual ~SettingsCommandProcessor(void);

	void PrepareContext(LoadingContext & context) override;

	void ProcessCommand(LoadingContext & context, const std::string & command, std::istream & stream) override;

};

