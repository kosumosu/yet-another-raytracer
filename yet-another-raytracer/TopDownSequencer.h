#pragma once

#include <utility>


#include "IBucketSequencer.h"


class TopDownSequencer : public IBucketSequencer
{
private:
	class TopDownSequence : public IBucketSequence
	{
	public:

		explicit TopDownSequence(uint_vector2 gridSize)
			: gridSize_(std::move(gridSize))
		{
		}


		std::optional<uint_vector2> getNext() override
		{
			if (row_ >= gridSize_[1])
				return std::nullopt;
			
			const uint_vector2 result{ column_, row_ };
			
			++column_;
			if (column_ == gridSize_[0])
			{
				column_ = 0;
				++row_;
			}

			return { result };
		}
		
	private:
		const uint_vector2 gridSize_;
		unsigned int row_ = 0;
		unsigned int column_ = 0;
	};

public:
	[[nodiscard]] std::unique_ptr<IBucketSequence> CreateSequence(uint_vector2 gridSize) const override
	{
		return std::make_unique<TopDownSequence>(std::move(gridSize));
	}
};
