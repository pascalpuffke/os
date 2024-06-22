#pragma once

#include <stdlib/types.h>

namespace Kernel {

class PIT final {
public:
    PIT() = default;

    enum class Channel : u8 {
        IRQ = 0,
        // 1 is now unused and hardware may not implement it
        Speaker = 2,
    };

    enum class AccessMode : u8 {
        LatchCount,
        LoByte,
        HiByte,
        LoHiByte,
    };

    enum class OperatingMode : u8 {
        InterruptOnSignalCount,
        RetriggerableOneShot,
        RateGenerator,
        SquareWaveGenerator,
        SoftwareTriggeredStrobe,
        HardwareTriggeredStrobe,
    };

    enum class BCDBinaryMode : u8 {
        Binary,
        BCD,
    };

    void enable(Channel, AccessMode, OperatingMode, BCDBinaryMode, i32 frequency);
    void set_count(Channel, u32 count);
    u32 count_for_frequency(u32 f);
    u32 read_count(Channel);

private:
};

}
