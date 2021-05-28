#pragma once

#include "Types.h"

#include <memory>
#include <optional>

class IBucketSequence
{
public:
	virtual ~IBucketSequence() = default;

	// Returns next bucket 2-dimensional index (ie.e coord in bucket grid space, not pixel coords)
	// Must support multithreaded access.
	virtual std::optional<uint_vector2> getNext() = 0;
};

class IBucketSequencer
{
public:
	virtual ~IBucketSequencer() = default;
	[[nodiscard]] virtual std::unique_ptr<IBucketSequence> CreateSequence(uint_vector2 gridSize) const = 0;
};
