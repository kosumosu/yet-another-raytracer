#pragma once

namespace accelerators {

template <class TMarcher>
class Accelerator
{
public:

	virtual ~Accelerator() = default;

	//virtual Marcher * StartMarching( const ray & ray, space_real near, space_real far ) const = 0;
	virtual TMarcher CreateMarcher() const = 0;
};

}