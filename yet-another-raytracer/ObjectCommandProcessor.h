#pragma once

#include "CommandProcessor.h"

#include "Types.h"
#include <vector>

class ObjectCommandProcessor : public CommandProcessor
{
public:
	ObjectCommandProcessor(void);
	virtual ~ObjectCommandProcessor(void);

	virtual void PrepareContext( LoadingContext & context );

	virtual void ProcessCommand( LoadingContext & context, const std::string & command, std::istream & stream );

private:
	std::vector<vector3> m_vertices;
};

