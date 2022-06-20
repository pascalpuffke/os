#pragma once

#include <stdlib/types.h>

namespace Kernel::VGA {

static constexpr usize width = 80;
static constexpr usize height = 25;

enum class Color : u8 {
    BLACK = 0,
    BLUE = 1,
    GREEN = 2,
    CYAN = 3,
    RED = 4,
    MAGENTA = 5,
    BROWN = 6,
    LIGHT_GREY = 7,
    DARK_GREY = 8,
    LIGHT_BLUE = 9,
    LIGHT_GREEN = 10,
    LIGHT_CYAN = 11,
    LIGHT_RED = 12,
    LIGHT_MAGENTA = 13,
    LIGHT_BROWN = 14,
    WHITE = 15,
};

static constexpr u8 entry_color(Color fg, Color bg)
{
    return static_cast<u8>(fg) | static_cast<u8>(bg) << 4;
}

static constexpr u16 entry(unsigned char uc, u8 color)
{
    return static_cast<u16>(uc) | static_cast<u16>(color) << 8;
}

}
