#include "ProcessTimeStopwatch.h"

#define NOGDI
#include "Windows.h"

unsigned long long FromFileTime(const FILETIME & fileTime)
{
	ULARGE_INTEGER largeInteger;
	largeInteger.LowPart = fileTime.dwLowDateTime;
	largeInteger.HighPart = fileTime.dwHighDateTime;
	return largeInteger.QuadPart;
}

void ProcessTimeStopwatch::Restart()
{
	FILETIME creationTime;
	FILETIME exitTime;
	FILETIME kernelTime;
	FILETIME userTime;
	GetProcessTimes(GetCurrentProcess(), &creationTime, &exitTime, &kernelTime, &userTime);
	_kernelStartTime = FromFileTime(kernelTime);
	_userStartTime = FromFileTime(userTime);
}

float ProcessTimeStopwatch::Sample() const
{
	FILETIME creationTime;
	FILETIME exitTime;
	FILETIME processKernelTime;
	FILETIME processUserTime;
	GetProcessTimes(GetCurrentProcess(), &creationTime, &exitTime, &processKernelTime, &processUserTime);
	
	const auto kernelTime = FromFileTime(processKernelTime) - _kernelStartTime;
	const auto userTime = FromFileTime(processUserTime) - _userStartTime;

	return float(kernelTime + userTime) * 0.000'000'1f;
}
