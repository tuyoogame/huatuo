#include "os/c-api/il2cpp-config-platforms.h"

#if !RUNTIME_TINY

#include "os/TimeZone.h"
#include "Allocator.h"

#include <string>

extern "C"
{
    int32_t UnityPalGetTimeZoneData(int32_t year, int64_t data[4], const char* names[2])
    {
        std::string namesBuffer[2];
        bool dst_inverted;
        int32_t result = il2cpp::os::TimeZone::GetTimeZoneData(year, data, namesBuffer, &dst_inverted);

        names[0] = Allocator::CopyToAllocatedStringBuffer(namesBuffer[0]);
        names[1] = Allocator::CopyToAllocatedStringBuffer(namesBuffer[1]);

        return result;
    }
}

#endif
