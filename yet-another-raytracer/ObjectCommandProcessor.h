#pragma once

#include "CommandProcessor.h"

#include "Types.h"
#include <vector>

class ObjectCommandProcessor : public CommandProcessor
{
public:
	ObjectCommandProcessor(void);
	virtual ~ObjectCommandProcessor(void);

	void PrepareContext(LoadingContext & context) override;

	void ProcessCommand(LoadingContext & context, const std::string & command, std::istream & stream) override;

private:
	std::vector<vector3> _vertices;
	std::vector<vector2> _uvs;
};

