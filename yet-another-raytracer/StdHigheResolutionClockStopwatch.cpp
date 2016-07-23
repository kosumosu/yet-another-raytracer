#include "StdHigheResolutionClockStopwatch.h"

StdHigheResolutionClockStopwatch::StdHigheResolutionClockStopwatch(void)
{
}


StdHigheResolutionClockStopwatch::~StdHigheResolutionClockStopwatch(void)
{
}

void StdHigheResolutionClockStopwatch::Restart()
{
	m_start = std::chrono::high_resolution_clock::now();
}

float StdHigheResolutionClockStopwatch::Sample() const
{
	auto duration = std::chrono::high_resolution_clock::now() - m_start;
	return float(std::chrono::duration_cast<std::chrono::microseconds>(duration).count()) / 1000000.0f;
}

