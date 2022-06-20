#pragma once

#include <stdlib/types.h>

namespace Kernel {

// Disable interrupts
ALWAYS_INLINE void cli()
{
    asm volatile("cli");
}

// Enable interrupts
ALWAYS_INLINE void sti()
{
    asm volatile("sti");
}

ALWAYS_INLINE void hang()
{
    while (1) {
        asm volatile("hlt");
    }
}

}
