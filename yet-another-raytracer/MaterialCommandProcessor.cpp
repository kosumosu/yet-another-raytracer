#include "MaterialCommandProcessor.h"

#include "BlinnMaterial.h"
#include "PareserHelper.h"
#include <typeinfo>

MaterialCommandProcessor::MaterialCommandProcessor(void)
{
}


MaterialCommandProcessor::~MaterialCommandProcessor(void)
{
}

void MaterialCommandProcessor::PrepareContext( LoadingContext & context )
{
	BlinnMaterial * material = new BlinnMaterial();

	m_last_ambient = color4(0.2f);
	material->emission(m_last_ambient);

	context.material(std::shared_ptr<Material>(material));
}

void MaterialCommandProcessor::ProcessCommand( LoadingContext & context, const std::string & command, std::istream & stream )
{
	BlinnMaterial * material;

	auto & type1 = typeid(*context.material().get());
	auto & type2 = typeid(BlinnMaterial);

	if (context.material().get() == nullptr || typeid(*context.material().get()) != typeid(BlinnMaterial))
	{
		material = new BlinnMaterial();
		context.material(std::shared_ptr<Material>(material));
		m_last_ambient = color4(0.2f);
		material->emission(m_last_ambient);
	}
	else
	{
		material = static_cast<BlinnMaterial *>(context.material().get());
	}

	if (command == "ambient")
	{
		auto new_ambient = color4(PareserHelper::ReadVec3(stream), 0.0f);
		material->emission(material->emission() - m_last_ambient + new_ambient);
		m_last_ambient = new_ambient;
	}
	else if (command == "emission")
	{
		auto new_emission = color4(PareserHelper::ReadVec3(stream), 0.0f);
		material->emission(new_emission + m_last_ambient);
	}
	else if (command == "diffuse")
	{
		auto new_diffuse = color4(PareserHelper::ReadVec3(stream), 0.0f);
		material->diffuse(new_diffuse);
	}
	else if (command == "specular")
	{
		auto new_specular = color4(PareserHelper::ReadVec3(stream), 0.0f);
		material->specular(new_specular);
	}
	else if (command == "shininess")
	{
		material->shininess(PareserHelper::ReadColorReal(stream));
	}
	else
	{
		throw std::exception();
	}
}
