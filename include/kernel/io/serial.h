#pragma once

#include <kernel/io/io.h>

namespace Kernel::IO::Serial {

bool initialize();
bool ready();
bool received();
bool is_transmit_empty();
u8 read();
void write(u8);
void write_string(const char*, usize);

}
