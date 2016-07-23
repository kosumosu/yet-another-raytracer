#include "GeometryLightSource.h"
#include "GeometryObject.h"
#include "Material.h"

GeometryLightSource::GeometryLightSource(const std::vector<GeometryObject *> & objects)
{
	std::vector<std::pair<GeometryObject*, color_real>> objectsWithWeights(objects.size());

	std::transform(std::begin(objects), std::end(objects), std::begin(objectsWithWeights),
		[](GeometryObject * const object)
	{
		return std::pair<GeometryObject*, color_real> (object, color_real(object->GetOneSidedSurfaceArea()) * object->material()->GetAverageEmission());
	});

	_distribution = math::discrete_distribution<GeometryObject*, color_real>(std::begin(objectsWithWeights), std::end(objectsWithWeights));
}

void GeometryLightSource::IterateOverFluxes(const LightingContext & context, const RayEvaluator & rayEvaluator, math::UniformRandomBitGenerator<unsigned int> & randomEngine, const flux_func & job) const
{
	
}
