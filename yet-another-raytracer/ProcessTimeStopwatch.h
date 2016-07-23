#pragma once
#include "Stopwatch.h"
#include "Windows.h"


class ProcessTimeStopwatch : public Stopwatch
{
public:
	ProcessTimeStopwatch()
		: _kernelStartTime(0)
		, _userStartTime(0)
	{}

	void Restart() override;
	float Sample() const override;

private:
	unsigned long long _kernelStartTime;
	unsigned long long _userStartTime;
};
