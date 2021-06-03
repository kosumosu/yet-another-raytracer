#pragma once
#include "CommandProcessor.h"

#include <filesystem>


class TextureCommandProcessor : public CommandProcessor
{
public:
	explicit TextureCommandProcessor(const std::filesystem::path& sceneFileName): _sceneFileName(sceneFileName) {  }
	void PrepareContext(LoadingContext & context) override;
	void ProcessCommand(LoadingContext & context, const std::string & command, std::istream & stream) override;
private:
	const std::filesystem::path& _sceneFileName;
};
