#include "MaterialCommandProcessor.h"

#include "BlinnMaterial.h"
#include "DielectricMaterial.h"
#include "ParserHelper.h"
#include <typeinfo>

MaterialCommandProcessor::MaterialCommandProcessor(void)
{
}


MaterialCommandProcessor::~MaterialCommandProcessor(void)
{
}

void MaterialCommandProcessor::PrepareContext(LoadingContext & context)
{
	// TODO: fix memory leak
	m_blinnMaterial = std::make_shared<BlinnMaterial>();
	m_dielectricMaterial = std::shared_ptr<DielectricMaterial>(new DielectricMaterial(1.5, 1.0, color_rgbx(1.0)));

	m_last_ambient = color_rgbx(0.2f);
	m_blinnMaterial->emission(m_last_ambient);


	context.material(m_blinnMaterial);
}

void MaterialCommandProcessor::ProcessCommand(LoadingContext & context, const std::string & command, std::istream & stream)
{
	if (command == "blinn")
	{
		context.material(m_blinnMaterial);
	}
	else if (command == "ambient")
	{
		auto new_ambient = color_rgbx(ParserHelper::ReadColorRgb(stream), 0.0f);
		m_blinnMaterial->emission(m_blinnMaterial->emission() - m_last_ambient + new_ambient);
		m_last_ambient = new_ambient;
	}
	else if (command == "emission")
	{
		auto new_emission = color_rgbx(ParserHelper::ReadColorRgb(stream), 0.0f);
		m_blinnMaterial->emission(new_emission + m_last_ambient);
	}
	else if (command == "diffuse")
	{
		auto new_diffuse = color_rgbx(ParserHelper::ReadColorRgb(stream), 0.0f);
		m_blinnMaterial->diffuse(new_diffuse);
	}
	else if (command == "specular")
	{
		auto new_specular = color_rgbx(ParserHelper::ReadColorRgb(stream), 0.0f);
		m_blinnMaterial->specular(new_specular);
	}
	else if (command == "shininess")
	{
		m_blinnMaterial->shininess(ParserHelper::ReadColorReal(stream));
	}
	else if (command == "translucency")
	{
		m_blinnMaterial->translucency(color_rgbx(ParserHelper::ReadColorRgb(stream), 0.0f));
	}
	
	else if (command == "dielectric")
	{
		context.material(m_dielectricMaterial);
	}
	else if (command == "iorInside")
	{
		auto ior = ParserHelper::ReadSpaceReal(stream);
		m_dielectricMaterial->setIorInside(ior);
	}
	else if (command == "iorOutside")
	{
		auto ior = ParserHelper::ReadSpaceReal(stream);
		m_dielectricMaterial->setIorOutside(ior);
	}
	else if (command == "surfaceTransparency")
	{
		m_dielectricMaterial->setSurfaceTransparency(color_rgbx(ParserHelper::ReadColorRgb(stream), 0.0f));
	}
	else
	{
		throw std::exception();
	}
}
