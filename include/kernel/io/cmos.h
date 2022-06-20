#pragma once

#include <stdlib/types.h>

namespace Kernel::IO::CMOS {

u8 read(u8);
void write(u8, u8);

}
