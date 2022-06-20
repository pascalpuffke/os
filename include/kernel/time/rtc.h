#pragma once

#include <stdlib/types.h>

namespace Kernel::Time::RTC {

struct Time {
    u32 second;
    u32 minute;
    u32 hour;
    u32 day;
    u32 month;
    u32 year;
};

Time now();

}
