#include <chrono.h>
#include <time.h>

#ifdef _WIN32

double second()
{
    // On Windows, return processor time in seconds.
    return ((double)clock() / (double)CLK_TCK);
}

#else

#if defined(__MACH__) && defined(__APPLE__)
#include <mach/mach.h>
#include <mach/mach_time.h>
#endif

static double myWallTime()
{
#ifdef __APPLE__
    static double timeConvert = 0.0;
    if (timeConvert == 0.0)
    {
        mach_timebase_info_data_t timeBase;
        mach_timebase_info(&timeBase);
        // Compute conversion factor from mach_absolute_time units to seconds.
        timeConvert = (double)timeBase.numer / (double)timeBase.denom / 1000000000.0;
    }
    return mach_absolute_time() * timeConvert;
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    // Convert timespec (seconds and nanoseconds) to seconds.
    return (double)ts.tv_sec + 1.0e-9 * ((double)ts.tv_nsec);
#endif // __APPLE__
}

double second()
{
    return myWallTime();
}

#endif
