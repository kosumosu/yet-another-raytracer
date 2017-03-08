#include "SceneLoader.h"

#include "Parser.h"
#include <fstream>

#include "CameraCommandFactory.h"
#include "LightSourceCommandFactory.h"
#include "MaterialCommandFactory.h"
#include "ObjectCommandFactory.h"
#include "SettingsCommandFactory.h"
#include "TransformCommandFactory.h"
#include "TextureCommandFactory.h"

// Static members


void SceneLoader::AddFactory( std::vector< std::shared_ptr<CommandFactory> > & collection, CommandFactory * factory )
{
	collection.push_back(std::shared_ptr<CommandFactory>(factory));
}


SceneLoader * SceneLoader::CreateDefault()
{
	std::vector< std::shared_ptr<CommandFactory> > factories
	{
		std::make_shared<CameraCommandFactory>(),
		std::make_shared<LightSourceCommandFactory>(),
		std::make_shared<MaterialCommandFactory>(),
		std::make_shared<ObjectCommandFactory>(),
		std::make_shared<SettingsCommandFactory>(),
		std::make_shared<TransformCommandFactory>(),
		std::make_shared<TextureCommandFactory>()

	};
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


void SceneLoader::CreateProcessors(ProcessorMap & command_map, ProcessorCollection & processors, const std::wstring & sceneFileName ) const
{
	for (auto & factory : m_factories)
	{
		auto commands = factory->GetSupportedCommands();
		std::shared_ptr<CommandProcessor> processor(factory->CreateCommandProcessor(sceneFileName));

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
	CreateProcessors(command_map, processors, filename);

	LoadingContext context(&scene);
	PrepareContext(context, processors);

	Parser parser(&stream);

	while (parser.MoveToNextCommand())
	{
		auto command = command_map.at(parser.current_command());
		if (command == nullptr)
			throw std::exception();
		command->ProcessCommand(context, parser.current_command(), stream);
	}
}


