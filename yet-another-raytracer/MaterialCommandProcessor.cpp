#include "MaterialCommandProcessor.h"

#include "BlinnMaterial.h"
#include "DielectricMaterial.h"
#include "ParserHelper.h"
#include <typeinfo>

MaterialCommandProcessor::MaterialCommandProcessor()
	: _ambient(color_rgb::zero())
	  , _emission(color_rgb::zero())
	  , _diffuseMap(nullptr)
      , _diffuse(color_rgb::fill(1))
	  , _specular(color_rgb::zero())
	  , _shininess(0)
	  , _translucency(color_rgb::zero())
	  , _iorInside(1)
	  , _iorOutside(1)
	  , _surfaceTransparency(color_rgb::zero())
{
}

void MaterialCommandProcessor::PrepareContext(LoadingContext & context)
{
	_ambient = color_rgb::fill(0.2f);
	context.material(nullptr);
}

void MaterialCommandProcessor::ProcessCommand(LoadingContext & context, const std::string & command, std::istream & stream)
{
	if (command == "selectMaterial")
	{
		const auto id = ParserHelper::ReadId(stream);
		context.material(context.scene()->getMaterials().at(id).get());
	}
	else if (command == "blinn")
	{
		auto id = ParserHelper::ReadId(stream);

		auto material = std::make_unique<BlinnMaterial>(_emission + _ambient, _diffuseMap, _diffuse, _specular, _shininess, _translucency);
		const auto materialRawPointer = material.get();
		context.scene()->getMaterials().insert(std::make_pair(std::move(id), std::move(material)));

		// also select it automatically
		context.material(materialRawPointer);
	}
	else if (command == "ambient")
	{
		_ambient = ParserHelper::ReadColorRgb(stream);
	}
	else if (command == "emission")
	{
		_emission = ParserHelper::ReadColorRgb(stream);
	}
	else if (command == "diffuseMap")
	{
		const auto mapId = ParserHelper::ReadId(stream);
		_diffuseMap = context.scene()->getMaps().at(mapId).get();
	}
	else if (command == "diffuse")
	{
		_diffuse = ParserHelper::ReadColorRgb(stream);
	}
	else if (command == "specular")
	{
		_specular = ParserHelper::ReadColorRgb(stream);
	}
	else if (command == "shininess")
	{
		_shininess = ParserHelper::ReadColorReal(stream);
	}
	else if (command == "translucency")
	{
		_translucency = ParserHelper::ReadColorRgb(stream);
	}
	
	else if (command == "dielectric")
	{
		auto id = ParserHelper::ReadId(stream);

		auto material = std::make_unique<DielectricMaterial>(_iorInside, _iorOutside, _surfaceTransparency);
		const auto materialRawPointer = material.get();
		context.scene()->getMaterials().insert(std::make_pair(std::move(id), std::move(material)));

		// also select it automatically
		context.material(materialRawPointer);
	}
	else if (command == "iorInside")
	{
		_iorInside = ParserHelper::ReadSpaceReal(stream);
	}
	else if (command == "iorOutside")
	{
		_iorOutside = ParserHelper::ReadSpaceReal(stream);
	}
	else if (command == "surfaceTransparency")
	{
		_surfaceTransparency = ParserHelper::ReadColorRgb(stream);
	}
	else
	{
		throw std::exception();
	}
}
