#include "il2cpp-config.h"

#include "os/Time.h"
#include <rtc.h>
#include <kernel.h>

namespace il2cpp
{
namespace os
{
    uint32_t Time::GetTicksMillisecondsMonotonic()
    {
        SceRtcTick tick;
        sceRtcGetCurrentTick(&tick);
        return tick.tick / 1000;            // value is in microseconds
    }

    int64_t Time::GetTicks100NanosecondsMonotonic()
    {
        SceRtcTick tick;
        sceRtcGetCurrentTick(&tick);
        return tick.tick * 10;          // value is in microseconds
    }

    int64_t Time::GetTicks100NanosecondsDateTime()
    {
        SceRtcTick tick;
        sceRtcGetCurrentTick(&tick);
        return tick.tick * 10;          // value is in microseconds
    }

    static const int64_t kSecondsBetween1601And1970 = 11644473600LL;
    static const int64_t kSecondsTo100NanoSeconds = 10000000;

    int64_t Time::GetSystemTimeAsFileTime()
    {
        SceKernelTimeval currentTime;
        int getTimeOfDayResult = sceKernelGettimeofday(&currentTime);
        IL2CPP_ASSERT(getTimeOfDayResult == 0 && "sceKernelGettimeofday() failed");

        return kSecondsTo100NanoSeconds * (static_cast<int64_t>(currentTime.tv_sec) + kSecondsBetween1601And1970) + 10 * currentTime.tv_usec;
    }
}
}
