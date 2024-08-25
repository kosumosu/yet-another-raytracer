#include "KDTreeAccelerator.h"

#include "KDTreeMarcher.h"

namespace accelerators::kd_tree
{
    KDTreeAccelerator::KDTreeAccelerator(const ObjectCollection& objects)
    {
        bounding_box3 scene_box;
        std::vector<objects::GeometryObject*> objects_copy(objects.size());
        std::transform(std::begin(objects), std::end(objects), std::begin(objects_copy),
                       [](auto objectPtr) { return objectPtr.get(); });

        for (const auto& object : objects_copy)
        {
            scene_box.Include(object->bounding_box());
        }

        m_scene_box = scene_box;

        m_max_depth = m_root_node.Build(scene_box, std::move(objects_copy));
    }

    KDTreeMarcher KDTreeAccelerator::CreateMarcher() const
    {
        return KDTreeMarcher(m_scene_box, &m_root_node, m_max_depth);
    }
}
