#include "ObjectCommandProcessor.h"

#include "FlatTriangleObject.h"
#include "SphereObject.h"
#include "ParserHelper.h"
#include <memory>

ObjectCommandProcessor::ObjectCommandProcessor(void)
{
}


ObjectCommandProcessor::~ObjectCommandProcessor(void)
{
}

void ObjectCommandProcessor::PrepareContext( LoadingContext & context )
{
	// nothing to do
}

void ObjectCommandProcessor::ProcessCommand( LoadingContext & context, const std::string & command, std::istream & stream )
{
	if (command == "maxverts")
	{
		m_vertices.reserve(ParserHelper::ReadUInt(stream));
	}
	else if (command == "maxvertnorms")
	{
		throw std::exception();
	}
	else if (command == "vertex")
	{
		m_vertices.push_back(ParserHelper::ReadVec3(stream));
	}
	else if (command == "vertexnormal")
	{
		 throw std::exception();
	}
	else if (command == "tri")
	{
		unsigned int index0, index1, index2;
		stream >> index0 >> index1 >> index2;
		auto vertex0 = (context.transform() * vector4(m_vertices[index0], space_real(1.0))).reduce();
		auto vertex1 = (context.transform() * vector4(m_vertices[index1], space_real(1.0))).reduce();
		auto vertex2 = (context.transform() * vector4(m_vertices[index2], space_real(1.0))).reduce();

		FlatTriangleObject * triangle = new FlatTriangleObject(vertex0, vertex1, vertex2);
		triangle->material(std::shared_ptr<Material>(context.material()->Clone()));

		context.scene()->objects().push_back(std::shared_ptr<GeometryObject>(triangle));
	}
	else if (command == "trinormal")
	{
		throw std::exception();
	}
	else if (command == "sphere")
	{
		auto center = ParserHelper::ReadVec3(stream);
		space_real radius = ParserHelper::ReadSpaceReal(stream);
		SphereObject * sphere = new SphereObject();
		sphere->center(center);
		sphere->radius(radius);
		sphere->transform(context.transform());
		sphere->material(std::shared_ptr<Material>(context.material()->Clone()));

		context.scene()->objects().push_back(std::shared_ptr<GeometryObject>(sphere));
	}
	else
	{
		throw std::exception();
	}

}
