#include <kernel/interrupts/pit.h>
#include <kernel/io/io.h>
#include <kernel/util/interrupt_scope.h>
#include <kernel/util/kassert.h>
#include <kernel/util/kprintf.h>

static constexpr u32 PIT_OSCILLATOR_FREQ = 1193182;
static constexpr u16 PIT_CHANNEL_0 = 0x40;
static constexpr u16 PIT_CHANNEL_2 = 0x42;
static constexpr u16 PIT_CMD_REGISTER = 0x43;

namespace Kernel {

void PIT::enable(Channel c, AccessMode am, OperatingMode om, BCDBinaryMode bcdbin, i32 frequency) {
    const auto channel = static_cast<u8>(c);
    const auto accessMode = static_cast<u8>(am);
    const auto operatingMode = static_cast<u8>(om);
    const auto bcdBinaryMode = static_cast<u8>(bcdbin);

    const u8 command = bcdBinaryMode | operatingMode << 1 | accessMode << 4 | channel << 6;
    kprintln("Sending PIT command: 0x%X", command);
    IO::outb(PIT_CMD_REGISTER, command);

    const auto count = count_for_frequency(frequency);

    kprintln("Sending PIT count: %d for frequency %d Hz", count, frequency);
    set_count(c, count);
}

void PIT::set_count(Channel c, u32 count) {
    InterruptScope _;

    u16 channel = PIT_CHANNEL_0 + static_cast<u8>(c);

    IO::outb(channel, count & 0xFF);
    IO::outb(channel, (count & 0xFF00) >> 8);
}

u32 PIT::count_for_frequency(u32 f) {
    if (f < 20 || f > PIT_OSCILLATOR_FREQ) {
        panic("PIT cannot produce requested frequency (20..=1193182)");
    }

    return PIT_OSCILLATOR_FREQ / f;
}

u32 PIT::read_count(Channel c) {
    InterruptScope _;

    u16 channel = PIT_CHANNEL_0 + static_cast<u8>(c);
    u32 count = 0;

    IO::outb(PIT_CMD_REGISTER, 0);

    count = IO::inb(channel);
    count |= IO::inb(channel) << 8;

    return count;
}

}
