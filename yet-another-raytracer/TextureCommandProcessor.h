#pragma once
#include "CommandProcessor.h"

class TextureCommandProcessor : public CommandProcessor
{
public:
	explicit TextureCommandProcessor(const std::wstring & sceneFileName): _sceneFileName(sceneFileName) {  }
	void PrepareContext(LoadingContext & context) override;
	void ProcessCommand(LoadingContext & context, const std::string & command, std::istream & stream) override;
private:
	const std::wstring & _sceneFileName;
};
