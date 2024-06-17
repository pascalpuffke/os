#pragma once

#include <stdlib/types.h>

namespace Kernel::VBE {

struct [[gnu::packed]] Info {
    char signature[4];
    u16 version;
    u16 oem_string_ptr[2];
    u8 capabilities[4];
    u16 video_mode_ptr[2];
    u16 total_memory_64k_blocks;
};

Info read_vbe_info(u32 address) {
    auto* info = reinterpret_cast<Info*>(address);
    return *info;
}

}