#include "SettingsCommandProcessor.h"

#include "PareserHelper.h"

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
		context.scene()->viewport_width(PareserHelper::Read<unsigned int>(stream));
		context.scene()->viewport_height(PareserHelper::Read<unsigned int>(stream));
	}
	else if (command == "maxdepth")
	{
		context.scene()->max_trace_depth(PareserHelper::Read<unsigned int>(stream));
	}
	else if (command == "output")
	{
		context.scene()->output_filename(PareserHelper::Read<std::string>(stream));
	}
	else
	{
		throw std::exception();
	}
}
