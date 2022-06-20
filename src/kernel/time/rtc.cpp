#include <kernel/io/cmos.h>
#include <kernel/io/io.h>
#include <kernel/time/rtc.h>

namespace Kernel::Time::RTC {

static bool update_in_progress() { return IO::CMOS::read(0x0A) & 0x80; }

static u8 bcd_to_binary(u8 value) { return (value & 0x0F) + ((value >> 4) * 10); }

static Time read()
{
    Time time;
    while (update_in_progress()) {
        for (int i = 0; i < 1000; i++)
            IO::wait();
    }

    u8 status = IO::CMOS::read(0x0B);
    time.second = IO::CMOS::read(0x00);
    time.minute = IO::CMOS::read(0x02);
    time.hour = IO::CMOS::read(0x04);
    time.day = IO::CMOS::read(0x07);
    time.month = IO::CMOS::read(0x08);
    time.year = IO::CMOS::read(0x09);

    bool is_pm = time.hour & 0x80;

    if (!(status & 0x04)) {
        time.second = bcd_to_binary(time.second);
        time.minute = bcd_to_binary(time.minute);
        time.hour = bcd_to_binary(time.hour & 0x7F);
        time.day = bcd_to_binary(time.day);
        time.month = bcd_to_binary(time.month);
        time.year = bcd_to_binary(time.year);
    }

    if (!(status & 0x02)) {
        time.hour = time.hour & 0x7F;
        if (is_pm)
            time.hour = time.hour | 0x80;
    }

    time.year += 2000;
    return time;
}

Time now()
{
    return read();
}

}
