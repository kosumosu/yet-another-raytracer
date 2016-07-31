#pragma once

#include "CommandProcessor.h"

class CameraCommandProcessor : public CommandProcessor
{
public:
	CameraCommandProcessor(void);
	virtual ~CameraCommandProcessor(void);

	void PrepareContext(LoadingContext & context) override;

	void ProcessCommand(LoadingContext & context, const std::string & command, std::istream & stream) override;

};

