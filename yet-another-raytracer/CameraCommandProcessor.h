#pragma once

#include "CommandProcessor.h"

class CameraCommandProcessor : public CommandProcessor
{
public:
	CameraCommandProcessor(void);
	virtual ~CameraCommandProcessor(void);

	virtual void PrepareContext( LoadingContext & context );

	virtual void ProcessCommand( LoadingContext & context, const std::string & command, std::istream & stream );

};

