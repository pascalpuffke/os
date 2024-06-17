#pragma once

#include <stdlib/types.h>

namespace Kernel {

enum class CPUFlag : usize {
    Carry = 0x01,
    Parity = 0x04,
    Auxiliary = 0x10,
    Zero = 0x40,
    Sign = 0x80,
    Trap = 0x100,
    InterruptEnable = 0x200,
    Direction = 0x400,
    Overflow = 0x800,
    IOPrivilege = 0x3000,
    NestedTask = 0x4000,
    Resume = 0x10000,
    VirtualMode = 0x20000,
    AlignmentCheck = 0x40000,
    VirtualInterrupt = 0x80000,
    VirtualInterruptPending = 0x100000,
    CPUID = 0x200000,
};

[[noreturn]] void hang();

bool has_flag(CPUFlag);

void cli();
void sti();

usize eflags();

u64 rdtsc();

}
