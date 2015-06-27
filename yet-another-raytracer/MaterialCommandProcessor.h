#pragma once

#include "CommandProcessor.h"
#include "Types.h"

class __declspec(align(MM_ALIGNMENT)) MaterialCommandProcessor : public CommandProcessor
{
public:
	MaterialCommandProcessor(void);
	virtual ~MaterialCommandProcessor(void);

	virtual void PrepareContext( LoadingContext & context );

	virtual void ProcessCommand( LoadingContext & context, const std::string & command, std::istream & stream );

private:
	color_rgbx m_last_ambient;
};

