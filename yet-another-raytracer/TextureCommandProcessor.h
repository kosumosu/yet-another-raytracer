#pragma once
#include "CommandProcessor.h"

#include <experimental/filesystem>


class TextureCommandProcessor : public CommandProcessor
{
public:
    explicit TextureCommandProcessor(const std::experimental::filesystem::path& sceneFileName): _sceneFileName(sceneFileName) {  }
	void PrepareContext(LoadingContext & context) override;
	void ProcessCommand(LoadingContext & context, const std::string & command, std::istream & stream) override;
private:
    const std::experimental::filesystem::path& _sceneFileName;
};
