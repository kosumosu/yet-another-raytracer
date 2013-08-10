#include "TransformCommandProcessor.h"

#include "PareserHelper.h"

TransformCommandProcessor::TransformCommandProcessor(void)
{
}


TransformCommandProcessor::~TransformCommandProcessor(void)
{
}


void TransformCommandProcessor::PrepareContext( LoadingContext & context )
{
	context.transform(matrix4());
}

void TransformCommandProcessor::ProcessCommand( LoadingContext & context, const std::string & command, std::istream & stream )
{
	if (command == "translate")
	{
		auto transform = math::translate(PareserHelper::ReadVec3(stream));
		context.transform(context.transform() * transform);
	}
	else if (command == "scale")
	{
		auto transform = math::scale(PareserHelper::ReadVec3(stream));
		context.transform(context.transform() * transform);
	}
	else if (command == "rotate")
	{
		auto axis = PareserHelper::ReadVec3(stream);
		space_real angle = PareserHelper::ReadSpaceReal(stream);
		auto transform = math::rotate_degrees(axis, angle);
		context.transform(context.transform() * transform);
	}
	else if (command == "pushTransform")
	{
		m_transform_stack.push(context.transform());
	}
	else if (command == "popTransform")
	{
		context.transform(m_transform_stack.peek());
		m_transform_stack.pop();
	}
	else
	{
		throw std::exception();
	}
}


