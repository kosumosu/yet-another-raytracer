#pragma once

#include "NullMarcher.h"
#include "../Accelerator.h"
#include <vector>



namespace accelerators::null
{
	class NullAccelerator final : public Accelerator<NullMarcher>
	{
	public:
		explicit NullAccelerator(const ObjectCollection & objects);
		~NullAccelerator() override = default;

		[[nodiscard]] NullMarcher CreateMarcher() const override;

	private:
		std::vector<objects::GeometryObject*> m_objects;
	};
}
