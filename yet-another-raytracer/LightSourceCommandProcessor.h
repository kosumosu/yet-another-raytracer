#pragma once

#include "CommandProcessor.h"
#include "Attenuation.h"

class LightSourceCommandProcessor : public CommandProcessor
{
public:
	LightSourceCommandProcessor(void);
	virtual ~LightSourceCommandProcessor(void);

	virtual void PrepareContext( LoadingContext & context );

	virtual void ProcessCommand( LoadingContext & context, const std::string & command, std::istream & stream );

private:
	Attenuation m_attenuation;
};

