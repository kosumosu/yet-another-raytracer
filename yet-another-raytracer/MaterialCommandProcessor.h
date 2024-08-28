#pragma once

#include "CommandProcessor.h"
#include "Types.h"
#include <memory>

class BlinnMaterial;
class DielectricMaterial;

class MaterialCommandProcessor : public CommandProcessor
{
public:
	MaterialCommandProcessor();

	void PrepareContext(LoadingContext & context) override;

	void ProcessCommand(LoadingContext & context, const std::string & command, std::istream & stream) override;

private:
	color_rgb _ambient;
	color_rgb _emission;
	const Texture *  _diffuseMap;
	color_rgb _diffuse;
	color_rgb _specular;
	color_real _shininess;
	color_rgb _translucency;

	space_real _iorInside;
	space_real _iorOutside;
	color_rgb _surfaceTransparency;

};

