#include "ProcessTimeStopwatch.h"

namespace {

        struct Time
        {
                // In microseconds
                unsigned long long kernel;
                unsigned long long user;
        };

#if defined(_WIN32)
#define NOGDI
#include "Windows.h"


        unsigned long long FromFileTime(const FILETIME& fileTime)
        {
                ULARGE_INTEGER largeInteger;
                largeInteger.LowPart = fileTime.dwLowDateTime;
                largeInteger.HighPart = fileTime.dwHighDateTime;
                return largeInteger.QuadPart;
        }


        Time getCurrentTime()
        {
                FILETIME creationTime;
                FILETIME exitTime;
                FILETIME kernelTime;
                FILETIME userTime;
                GetProcessTimes(GetCurrentProcess(), &creationTime, &exitTime, &kernelTime, &userTime);
                return { FromFileTime(kernelTime), FromFileTime(userTime) };
        }

#elif defined(__unix__) || defined(__unix) || defined(unix) || (defined(__APPLE__) && defined(__MACH__))
#include <unistd.h>
#include <sys/resource.h>
#include <sys/times.h>
#include <time.h>

        Time getCurrentTime()
        {
#if defined(RUSAGE_SELF)
                {
                        struct rusage rusage;
                        if (getrusage(RUSAGE_SELF, &rusage) != -1)
                        {
                                return { rusage.ru_stime.tv_sec * 1'000'000 + rusage.ru_stime.tv_usec, rusage.ru_utime.tv_sec * 1'000'000 + rusage.ru_utime.tv_usec };
                        }
                }
#else
#error "Implement getCurrentTime() for your system."
#endif

        }

#else
#error "Unable to define getCPUTime( ) for an unknown OS."
#endif
}

void ProcessTimeStopwatch::Restart()
{
        const auto time = getCurrentTime();
        _kernelStartTime = time.kernel;
        _userStartTime = time.user;
}

float ProcessTimeStopwatch::Sample() const
{
        const auto time = getCurrentTime();

        const auto kernelTime = time.kernel - _kernelStartTime;
        const auto userTime = time.user - _userStartTime;

        return float(kernelTime + userTime) * 0.000'000'1f;
}
