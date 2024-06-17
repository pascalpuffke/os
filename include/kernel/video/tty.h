#pragma once

#include <stdlib/types.h>

namespace Kernel::VGA {

enum class Color : u8;

}

namespace Kernel::TTY {

void initialize();
void reset_color();
void set_color(VGA::Color, VGA::Color);
bool is_initialized();
int put_entry_at(char, u8 color, usize, usize);
int put_char(char);
int write(const char*, usize);

namespace Cursor {

    void enable(u8 start, u8 end);
    void disable();
    void move(usize, usize);
    u16 position();

}

}
