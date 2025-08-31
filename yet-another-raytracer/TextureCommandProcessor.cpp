#include "TextureCommandProcessor.h"
#include "ParserHelper.h"
#include "SolidColorTexture.h"
#include "VisualizeUVTexture.h"
#include "BitmapTexture.h"

#include <filesystem>

namespace fs = std::filesystem;

fs::path getPathRelativeToScene(const fs::path & sceneFilePath, const fs::path & relative)
{
	return sceneFilePath.parent_path() / relative;
}

void TextureCommandProcessor::PrepareContext(LoadingContext & context)
{
	
}

void TextureCommandProcessor::ProcessCommand(LoadingContext & context, const std::string & command, std::istream & stream)
{
	if (command == "solidColorTexture")
	{
		const auto id = ParserHelper::ReadId(stream);
		const auto color = color_rgbx(ParserHelper::ReadColorRgb(stream), 0.0f);

		context.scene()->getMaps().insert(std::make_pair(id, std::make_shared<SolidColorTexture>(color)));
	}
	else if (command == "bitmapTexture")
	{
		const auto id = ParserHelper::ReadId(stream);
		//ParserHelper::Read<std::string>(stream); // This is a workaround for the bug "std::quoted broken in 15.8, fails to properly extract characters from input stream after encountering delimiter" https://developercommunity.visualstudio.com/content/problem/314993/stdquoted-broken-in-158-fails-to-properly-extract.html
		const auto fileName = getPathRelativeToScene(_sceneFileName, ParserHelper::ReadFileName(stream));

		context.scene()->getMaps().insert(std::make_pair(id, std::make_shared<BitmapTexture>(fileName, vector2::zero())));
	}
	else if (command == "visualizeUVTexture")
	{
		const auto id = ParserHelper::ReadId(stream);

		context.scene()->getMaps().insert(std::make_pair(id, std::make_shared<VisualizeUVTexture>()));
	}
	else if (command == "nullTexture")
	{
		const auto id = ParserHelper::ReadId(stream);

		context.scene()->getMaps().insert(std::make_pair(id, std::shared_ptr<Texture>()));
	}
	else
	{
		throw std::exception();
	}
}
