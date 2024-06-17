#pragma once

#include <stdlib/types.h>

namespace Kernel::IO {

inline void outb(u16 port, u8 value) {
    asm volatile("outb %0, %1"
                 :
                 : "a"(value), "Nd"(port));
}

inline void outw(u16 port, u16 value) {
    asm volatile("outw %0, %1"
                 :
                 : "a"(value), "Nd"(port));
}

inline void outl(u16 port, u32 value) {
    asm volatile("outl %0, %1"
                 :
                 : "a"(value), "Nd"(port));
}

inline u8 inb(u16 port) {
    u8 value;
    asm volatile("inb %1, %0"
                 : "=a"(value)
                 : "Nd"(port));
    return value;
}

inline u16 inw(u16 port) {
    u16 value;
    asm volatile("inw %1, %0"
                 : "=a"(value)
                 : "Nd"(port));
    return value;
}

inline u32 inl(u16 port) {
    u32 value;
    asm volatile("inl %1, %0"
                 : "=a"(value)
                 : "Nd"(port));
    return value;
}

inline void wait() {
    outb(0x80, 0x00);
}

inline void wait_for(usize cycles) {
    for (usize i = 0; i < cycles; i++)
        wait();
}

}
