#pragma once

#include "CommandProcessor.h"
#include "Attenuation.h"

class LightSourceCommandProcessor : public CommandProcessor
{
public:
	LightSourceCommandProcessor(void);
	virtual ~LightSourceCommandProcessor(void);

	void PrepareContext(LoadingContext & context) override;

	void ProcessCommand(LoadingContext & context, const std::string & command, std::istream & stream) override;

private:
	Attenuation m_attenuation;
};

