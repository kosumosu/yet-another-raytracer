#pragma once
#include "CommandProcessor.h"

class MapCommandProcessor : public CommandProcessor
{
public:
	void PrepareContext(LoadingContext & context) override;
	void ProcessCommand(LoadingContext & context, const std::string & command, std::istream & stream) override;
};
