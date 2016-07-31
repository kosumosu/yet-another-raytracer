#pragma once

#include "CommandProcessor.h"
#include "TransformStack.h"
#include "Types.h"

class TransformCommandProcessor : public CommandProcessor
{
public:
	TransformCommandProcessor(void);
	virtual ~TransformCommandProcessor(void);

	void PrepareContext(LoadingContext & context) override;

	void ProcessCommand(LoadingContext & context, const std::string & command, std::istream & stream) override;

private:
	TransformStack m_transform_stack;
};

