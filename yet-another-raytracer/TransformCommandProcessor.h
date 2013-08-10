#pragma once

#include "CommandProcessor.h"
#include "TransformStack.h"
#include "Types.h"

class TransformCommandProcessor : public CommandProcessor
{
public:
	TransformCommandProcessor(void);
	virtual ~TransformCommandProcessor(void);

	virtual void PrepareContext( LoadingContext & context );

	virtual void ProcessCommand( LoadingContext & context, const std::string & command, std::istream & stream );

private:
	TransformStack m_transform_stack;
};

