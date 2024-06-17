#pragma once

// This codebase uses aggressive warning settings and GCC loses its mind over the external multiboot header.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"

#include <kernel/multiboot/multiboot.h>

#pragma GCC diagnostic pop

#include <stdlib/optional.h>
#include <stdlib/types.h>

/*
 * DOCUMENTATION:
 * https://www.gnu.org/software/grub/manual/multiboot/multiboot.html
 */

namespace Kernel {

enum class MultibootFlag : multiboot_uint32_t {
    MEMORY = (1 << 0),
    BOOT_DEVICE = (1 << 1),
    CMDLINE = (1 << 2),
    MODULES = (1 << 3),
    AOUT_SYMBOL_TABLE = (1 << 4),
    ELF_SECTION_TABLE = (1 << 5),
    MEMORY_MAP = (1 << 6),
    DRIVES = (1 << 7),
    CONFIG_TABLE = (1 << 8),
    BOOT_LOADER_NAME = (1 << 9),
    APM_TABLE = (1 << 10),
    VBE = (1 << 11),
    FRAMEBUFFER = (1 << 12),
};

struct MultibootMemory {
    u32 lower; // At addr 0x0
    u32 upper; // At addr 0x100000
};

struct MultibootBootDevice {
    u32 drive;
    u32 part_1;
    u32 part_2;
    u32 part_3;
};

struct MultibootModule {
    u32 start;
    u32 end;
    const char* string;
};

struct MultibootElfSection {
    u32 num;
    u32 size;
    const char* addr;
    u32 shndx;
};

struct MultibootVBE {
    u32 control_info;
    u32 mode_info;
    u16 mode;
    u16 interface_seg;
    u16 interface_offset;
    u16 interface_length;
};

enum class MultibootFramebufferType : u8 {
    INDEXED = 0,
    RGB = 1,
    EGA_TEXT = 2,
};

static const char* framebuffer_type_to_string(MultibootFramebufferType type) {
    return (const char*[]) { "Indexed", "RGB", "EGA text mode" }[static_cast<u8>(type)];
}

struct MultibootFramebuffer {
    u64 address;
    u32 pitch;
    u32 width;
    u32 height;
    u8 depth;
    MultibootFramebufferType type;
};

class Multiboot final {
public:
    explicit Multiboot(const multiboot_info_t*);

    [[nodiscard]] bool has_flag(MultibootFlag) const;

    [[nodiscard]] Optional<const char*> cmdline() const;
    [[nodiscard]] Optional<const char*> boot_loader_name() const;
    [[nodiscard]] Optional<MultibootVBE> vbe() const;
    [[nodiscard]] Optional<MultibootFramebuffer> framebuffer() const;

private:
    const multiboot_info_t* m_mbd;
};

}
