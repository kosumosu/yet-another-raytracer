#include "MapCommandProcessor.h"
#include "ParserHelper.h"
#include "SolidColorMap.h"
#include "VisualizeUVMap.h"

void MapCommandProcessor::PrepareContext(LoadingContext & context)
{
	
}

void MapCommandProcessor::ProcessCommand(LoadingContext & context, const std::string & command, std::istream & stream)
{
	if (command == "solidColor")
	{
		const auto id = ParserHelper::ReadId(stream);
		const auto color = color_rgbx(ParserHelper::ReadColorRgb(stream), 0.0f);

		context.scene()->getMaps().insert(std::make_pair(id, std::make_shared<SolidColorMap>(color)));
	}
	else if (command == "visualizeUV")
	{
		const auto id = ParserHelper::ReadId(stream);

		context.scene()->getMaps().insert(std::make_pair(id, std::make_shared<VisualizeUVMap>()));
	}
	else if (command == "nullMap")
	{
		const auto id = ParserHelper::ReadId(stream);

		context.scene()->getMaps().insert(std::make_pair(id, std::shared_ptr<Map>()));
	}
	else
	{
		throw std::exception();
	}
}
