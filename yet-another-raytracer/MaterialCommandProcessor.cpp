#include "MaterialCommandProcessor.h"

#include "BlinnMaterial.h"
#include "DielectricMaterial.h"
#include "ParserHelper.h"
#include <typeinfo>

MaterialCommandProcessor::MaterialCommandProcessor(void)
	: _ambient(color_rgbx::zero())
	  , _emission(color_rgbx::zero())
	  , _diffuseMap(nullptr)
      , _diffuse(color_rgbx::fill(1))
	  , _specular(color_rgbx::zero())
	  , _shininess(0)
	  , _translucency(color_rgbx::zero())
	  , _iorInside(1)
	  , _iorOutside(1)
	  , _surfaceTransparency(color_rgbx::zero())
{
}

void MaterialCommandProcessor::PrepareContext(LoadingContext & context)
{
	_ambient = color_rgbx::fill(0.2f);
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
		_ambient = color_rgbx(ParserHelper::ReadColorRgb(stream), 0.0f);
	}
	else if (command == "emission")
	{
		_emission = color_rgbx(ParserHelper::ReadColorRgb(stream), 0.0f);
	}
	else if (command == "diffuseMap")
	{
		const auto mapId = ParserHelper::ReadId(stream);
		_diffuseMap = context.scene()->getMaps().at(mapId).get();
	}
	else if (command == "diffuse")
	{
		_diffuse = color_rgbx(ParserHelper::ReadColorRgb(stream), 0.0f);
	}
	else if (command == "specular")
	{
		_specular = color_rgbx(ParserHelper::ReadColorRgb(stream), 0.0f);
	}
	else if (command == "shininess")
	{
		_shininess = ParserHelper::ReadColorReal(stream);
	}
	else if (command == "translucency")
	{
		_translucency = color_rgbx(ParserHelper::ReadColorRgb(stream), 0.0f);
	}
	
	else if (command == "dielectric")
	{
		const auto id = ParserHelper::ReadId(stream);

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
		_surfaceTransparency = color_rgbx(ParserHelper::ReadColorRgb(stream), 0.0f);
	}
	else
	{
		throw std::exception();
	}
}
