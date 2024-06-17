#pragma once

#include <stdlib/types.h>

namespace Kernel {

struct GDT {
    u16 limit;
    u64 base;
} __attribute__((packed));

void encode_gdt(u8* target, const GDT* source) {
    target[0] = source->limit & 0xFF;
    target[1] = (source->limit >> 8) & 0xFF;
    target[6] = (source->limit >> 16) & 0xFF;
}

}
