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
		_vertices.reserve(ParserHelper::ReadUInt(stream));
	}
	else if (command == "maxvertnorms")
	{
		throw std::exception();
	}
	else if (command == "vertex")
	{
		_vertices.push_back(ParserHelper::ReadVec3(stream));
	}
	else if (command == "uv")
	{
		_uvs.push_back(ParserHelper::ReadVec2(stream));
	}
	else if (command == "vertexnormal")
	{
		 throw std::exception();
	}
	else if (command == "tri")
	{
		const unsigned int index0 = ParserHelper::ReadUInt(stream); 
		const unsigned int index1 = ParserHelper::ReadUInt(stream);
		const unsigned int index2 = ParserHelper::ReadUInt(stream);

		auto vertex0 = (context.transform() * vector4(_vertices[index0], space_real(1.0))).reduce();
		auto vertex1 = (context.transform() * vector4(_vertices[index1], space_real(1.0))).reduce();
		auto vertex2 = (context.transform() * vector4(_vertices[index2], space_real(1.0))).reduce();

		uvs_t zeroUVs({ vector2::zero() });

		FlatTriangleObject * triangle = new FlatTriangleObject(vertex0, vertex1, vertex2, zeroUVs, zeroUVs, zeroUVs);
		triangle->material(context.material());

		context.scene()->objects().push_back(std::shared_ptr<GeometryObject>(triangle));
	}
	else if (command == "triUV")
	{
		const unsigned int index0 = ParserHelper::ReadUInt(stream);
		const unsigned int index1 = ParserHelper::ReadUInt(stream);
		const unsigned int index2 = ParserHelper::ReadUInt(stream);

		const unsigned int uvIndex0 = ParserHelper::ReadUInt(stream);
		const unsigned int uvIndex1 = ParserHelper::ReadUInt(stream);
		const unsigned int uvIndex2 = ParserHelper::ReadUInt(stream);

		const auto vertex0 = (context.transform() * vector4(_vertices[index0], space_real(1.0))).reduce();
		const auto vertex1 = (context.transform() * vector4(_vertices[index1], space_real(1.0))).reduce();
		const auto vertex2 = (context.transform() * vector4(_vertices[index2], space_real(1.0))).reduce();

		const vector2 vertex0uv = _uvs[uvIndex0];
		const vector2 vertex1uv = _uvs[uvIndex1];
		const vector2 vertex2uv = _uvs[uvIndex2];

		FlatTriangleObject * triangle = new FlatTriangleObject(vertex0, vertex1, vertex2, uvs_t({vertex0uv}), uvs_t({ vertex1uv }), uvs_t({ vertex2uv }));
		triangle->material(context.material());

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
		sphere->material(context.material());

		context.scene()->objects().push_back(std::shared_ptr<GeometryObject>(sphere));
	}
	else
	{
		throw std::exception();
	}

}
