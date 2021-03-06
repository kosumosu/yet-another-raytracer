#pragma once

#include "Scene.h"
#include "CommandFactory.h"
#include "CommandProcessor.h"
#include <map>
#include <memory>
#include <filesystem>


class SceneLoader
{
public:

	static SceneLoader * CreateDefault();

	SceneLoader(const std::vector< std::shared_ptr<CommandFactory> > & factories);
	~SceneLoader(void);

	void Load(Scene & scene, const std::filesystem::path& filename) const;

private:
	std::vector< std::shared_ptr<CommandFactory> > m_factories;
	void CreateProcessors(ProcessorMap & command_map, ProcessorCollection & processors, const std::filesystem::path& sceneFileName) const;
	void PrepareContext(LoadingContext & context, const ProcessorCollection & processors) const;

	static void AddFactory(std::vector< std::shared_ptr<CommandFactory> > & collection, CommandFactory * factory);
};

