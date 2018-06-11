#pragma once

#include "Stopwatch.h"
#include <chrono>

class StdHigheResolutionClockStopwatch : public Stopwatch
{
public:
	void Restart() override;

	float Sample() const override;

private:
	std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
};

