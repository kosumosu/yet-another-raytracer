#include "SettingsCommandProcessor.h"

#include "ParserHelper.h"

SettingsCommandProcessor::SettingsCommandProcessor(void)
{
}


SettingsCommandProcessor::~SettingsCommandProcessor(void)
{
}

void SettingsCommandProcessor::PrepareContext( LoadingContext & context )
{
	context.scene()->viewport_width(640);
	context.scene()->viewport_height(480);
	context.scene()->max_trace_depth(5);
	context.scene()->output_filename("out.png");
}

void SettingsCommandProcessor::ProcessCommand( LoadingContext & context, const std::string & command, std::istream & stream )
{
	if (command == "size")
	{
		context.scene()->viewport_width(ParserHelper::Read<unsigned int>(stream));
		context.scene()->viewport_height(ParserHelper::Read<unsigned int>(stream));
	}
	else if (command == "maxdepth")
	{
		context.scene()->max_trace_depth(ParserHelper::Read<unsigned int>(stream));
	}
	else if (command == "output")
	{
		context.scene()->output_filename(ParserHelper::Read<std::string>(stream));
	}
	else if (command == "background")
	{
		context.scene()->setEnvironmentColor(ParserHelper::ReadColorRgb(stream));
	}
	else if (command == "samples")
	{
		context.scene()->setSamplesPerPixel(ParserHelper::ReadUInt(stream));
	}
	else if (command == "crop")
	{
		context.scene()->setCropX(ParserHelper::ReadUInt(stream));
		context.scene()->setCropY(ParserHelper::ReadUInt(stream));
		context.scene()->setCropWidth(ParserHelper::ReadUInt(stream));
		context.scene()->setCropHeight(ParserHelper::ReadUInt(stream));
	}
	else
	{
		throw std::exception();
	}
}
