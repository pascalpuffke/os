#pragma once

#include <kernel/io/io.h>
#include <stdlib/result.h>

namespace Kernel::IO::Serial {

Result<void> initialize();
bool ready();
bool received();
bool is_transmit_empty();
u8 read();
void write(u8);
void write_string(const char*, usize);

}
