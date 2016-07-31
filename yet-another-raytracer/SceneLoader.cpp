#include "SceneLoader.h"

#include "Parser.h"
#include <fstream>

#include "CameraCommandFactory.h"
#include "LightSourceCommandFactory.h"
#include "MaterialCommandFactory.h"
#include "ObjectCommandFactory.h"
#include "SettingsCommandFactory.h"
#include "TransformCommandFactory.h"

// Static members


void SceneLoader::AddFactory( std::vector< std::shared_ptr<CommandFactory> > & collection, CommandFactory * factory )
{
	collection.push_back(std::shared_ptr<CommandFactory>(factory));
}


SceneLoader * SceneLoader::CreateDefault()
{
	std::vector< std::shared_ptr<CommandFactory> > factories;

	AddFactory(factories, new CameraCommandFactory());
	AddFactory(factories, new LightSourceCommandFactory());
	AddFactory(factories, new MaterialCommandFactory());
	AddFactory(factories, new ObjectCommandFactory());
	AddFactory(factories, new SettingsCommandFactory());
	AddFactory(factories, new TransformCommandFactory());

	return new SceneLoader(factories);
}


// Non-static members

SceneLoader::SceneLoader(const std::vector< std::shared_ptr<CommandFactory> > & factories)
	: m_factories(factories)
{
}


SceneLoader::~SceneLoader( void )
{

}


void SceneLoader::CreateProcessors(ProcessorMap & command_map, ProcessorCollection & processors ) const
{
	for (auto & factory : m_factories)
	{
		auto commands = factory->GetSupportedCommands();
		std::shared_ptr<CommandProcessor> processor(factory->CreateCommandProcessor());

		processors.push_back(processor);

		for (auto & command : commands)
		{
			command_map[command] = processor;
		}
	}
	
}


void SceneLoader::PrepareContext(LoadingContext & context, const ProcessorCollection & processors) const
{
	for (auto & processor : processors)
	{
		processor->PrepareContext(context);
	}
}



void SceneLoader::Load(Scene & scene, const std::wstring & filename) const
{
	std::ifstream stream;

	stream.open(filename, std::ifstream::in);

	if (!stream.is_open())
		throw std::exception();

	stream >> std::boolalpha;

	ProcessorMap command_map;
	ProcessorCollection processors;
	CreateProcessors(command_map, processors);

	LoadingContext context(&scene);
	PrepareContext(context, processors);

	Parser parser(&stream);

	while (parser.MoveToNextCommand())
	{
		auto command = command_map[parser.current_command()];
		if (command == nullptr)
			throw std::exception();
		command->ProcessCommand(context, parser.current_command(), stream);
	}
}


