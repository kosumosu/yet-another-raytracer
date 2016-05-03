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

	virtual void PrepareContext( LoadingContext & context );

	virtual void ProcessCommand( LoadingContext & context, const std::string & command, std::istream & stream );

private:
	color_rgbx m_last_ambient;
	std::shared_ptr<BlinnMaterial> m_blinnMaterial;
	std::shared_ptr<DielectricMaterial> m_dielectricMaterial;
};

