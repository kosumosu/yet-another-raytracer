#include "TextureCommandFactory.h"
#include "TextureCommandProcessor.h"

std::vector<std::string> TextureCommandFactory::GetSupportedCommands() const
{
	return {"solidColorTexture", "bitmapTexture", "visualizeUVTexture", "nullTexture"};
}

CommandProcessor * TextureCommandFactory::CreateCommandProcessor(const std::wstring & sceneFileName) const
{
	return new TextureCommandProcessor(sceneFileName);
}
