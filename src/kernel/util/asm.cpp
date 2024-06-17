#include <kernel/util/asm.h>

namespace Kernel {

void cli() {
    asm volatile("cli");
}

void sti() {
    asm volatile("sti");
}

void hang() {
    while (true)
        asm volatile("hlt");
}

bool has_flag(CPUFlag flag) {
    return eflags() & static_cast<usize>(flag);
}

usize eflags() {
    usize flags;
    asm volatile("pushf; pop %0"
                 : "=r"(flags));
    return flags;
}

u64 rdtsc() {
    u32 lsw;
    u32 msw;
    asm volatile("rdtsc"
                 : "=d"(msw), "=a"(lsw));
    return static_cast<u64>(msw) << 32 | lsw;
}

}
