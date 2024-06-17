#include <kernel/multiboot/mb.h>
#include <libc/ctype.h>

namespace Kernel {

inline bool string_blank(const char* s) {
    while (*s) {
        if (!isspace(*s))
            return false;
        s++;
    }
    return true;
}

Multiboot::Multiboot(const multiboot_info_t* mbd)
    : m_mbd(mbd) {
}

bool Multiboot::has_flag(MultibootFlag flag) const {
    return (m_mbd->flags & static_cast<u32>(flag)) == static_cast<u32>(flag);
}

Optional<const char*> Multiboot::cmdline() const {
    if (!has_flag(MultibootFlag::CMDLINE))
        return Optional<const char*>::empty();
    const auto* cmdline = reinterpret_cast<const char*>(m_mbd->cmdline);
    if (string_blank(cmdline))
        return Optional<const char*>::empty();
    return cmdline;
}

Optional<const char*> Multiboot::boot_loader_name() const {
    if (!has_flag(MultibootFlag::BOOT_LOADER_NAME))
        return Optional<const char*>::empty();
    const auto* name = reinterpret_cast<const char*>(m_mbd->boot_loader_name);
    if (string_blank(name))
        return Optional<const char*>::empty();
    return name;
}

Optional<MultibootVBE> Multiboot::vbe() const {
    if (!has_flag(MultibootFlag::VBE))
        return Optional<MultibootVBE>::empty();
    auto VBE = MultibootVBE {
        .control_info = m_mbd->vbe_control_info,
        .mode_info = m_mbd->vbe_mode_info,
        .mode = m_mbd->vbe_mode,
        .interface_seg = m_mbd->vbe_interface_seg,
        .interface_offset = m_mbd->vbe_interface_off,
        .interface_length = m_mbd->vbe_interface_len,
    };
    return VBE;
}

Optional<MultibootFramebuffer> Multiboot::framebuffer() const {
    if (!has_flag(MultibootFlag::FRAMEBUFFER))
        return Optional<MultibootFramebuffer>::empty();
    auto framebuffer = MultibootFramebuffer {
        .address = m_mbd->framebuffer_addr,
        .pitch = m_mbd->framebuffer_pitch,
        .width = m_mbd->framebuffer_width,
        .height = m_mbd->framebuffer_height,
        .depth = m_mbd->framebuffer_bpp,
        .type = static_cast<MultibootFramebufferType>(m_mbd->framebuffer_type),
    };
    return framebuffer;
}

}
