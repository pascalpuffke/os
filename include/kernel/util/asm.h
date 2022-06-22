#pragma once

#include <stdlib/types.h>

namespace Kernel {

static constexpr usize CPU_FLAG_CF = 0x01; // Carry flag
static constexpr usize CPU_FLAG_PF = 0x04; // Parity flag
static constexpr usize CPU_FLAG_AF = 0x10; // Auxiliary flag
static constexpr usize CPU_FLAG_ZF = 0x40; // Zero flag
static constexpr usize CPU_FLAG_SF = 0x80; // Sign flag
static constexpr usize CPU_FLAG_TF = 0x100; // Trap flag
static constexpr usize CPU_FLAG_IF = 0x200; // Interrupt enable flag
static constexpr usize CPU_FLAG_DF = 0x400; // Direction flag
static constexpr usize CPU_FLAG_OF = 0x800; // Overflow flag
static constexpr usize CPU_FLAG_IOPL = 0x3000; // I/O privilege level
static constexpr usize CPU_FLAG_NT = 0x4000; // Nested task flag
static constexpr usize CPU_FLAG_RF = 0x10000; // Resume flag
static constexpr usize CPU_FLAG_VM = 0x20000; // Virtual 8086 mode flag
static constexpr usize CPU_FLAG_AC = 0x40000; // Alignment check
static constexpr usize CPU_FLAG_VIF = 0x80000; // Virtual interrupt flag
static constexpr usize CPU_FLAG_VIP = 0x100000; // Virtual interrupt pending
static constexpr usize CPU_FLAG_ID = 0x200000; // Able to use CPUID instruction

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

// Halt the CPU
ALWAYS_INLINE void hang()
{
    while (1) {
        asm volatile("hlt");
    }
}

// CPU flags register
ALWAYS_INLINE usize eflags()
{
    usize flags;
    asm volatile("pushf; pop %0"
                 : "=r"(flags));
    return flags;
}

}
