#include <kernel/io/cmos.h>
#include <kernel/io/io.h>

namespace Kernel::IO::CMOS {

u8 read(u8 index)
{
    IO::outb(0x70, index);
    return IO::inb(0x71);
}

void write(u8 index, u8 value)
{
    IO::outb(0x70, index);
    IO::outb(0x71, value);
}

}
