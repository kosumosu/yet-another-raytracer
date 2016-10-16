#pragma once

#include "CommandProcessor.h"
#include "Types.h"
#include <memory>

class BlinnMaterial;
class DielectricMaterial;

class MaterialCommandProcessor : public CommandProcessor
{
public:
	MaterialCommandProcessor(void);
	virtual ~MaterialCommandProcessor(void);

	void PrepareContext(LoadingContext & context) override;

	void ProcessCommand(LoadingContext & context, const std::string & command, std::istream & stream) override;

private:
	color_rgbx _ambient;
	color_rgbx _emission;
	const Map *  _diffuseMap;
	color_rgbx _diffuse;
	color_rgbx _specular;
	color_real _shininess;
	color_rgbx _translucency;

	space_real _iorInside;
	space_real _iorOutside;
	color_rgbx _surfaceTransparency;

};

