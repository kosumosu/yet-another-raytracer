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
	color_rgbx m_last_ambient;
	std::shared_ptr<BlinnMaterial> m_blinnMaterial;
	std::shared_ptr<DielectricMaterial> m_dielectricMaterial;
};

