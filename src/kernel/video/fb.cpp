#include <kernel/heap/kmalloc.h>
#include <kernel/util/kassert.h>
#include <kernel/util/kprintf.h>
#include <kernel/video/fb.h>
#include <libc/string.h>

namespace Kernel {

Framebuffer::Framebuffer(u8* hw_buffer, u32 width, u32 height, u32 pitch, u32 depth)
    : m_front_buffer(hw_buffer)
    , m_width(width)
    , m_height(height)
    , m_pitch(pitch)
    , m_depth(depth) {
    m_buffer_size = pitch * height;

    kprintln("Creating framebuffer %dx%d %dbpp size %d", width, height, depth, m_buffer_size);
    m_back_buffer = static_cast<u8*>(kmalloc(m_buffer_size));

    if (!m_back_buffer) {
        panic("Framebuffer allocation failed. Ensure the memory manager has enough resources available to handle the requested resolution.");
    }

    clear();

    kprintln("Created framebuffer. front=%p back=%p", m_front_buffer, m_back_buffer);
}

Framebuffer::~Framebuffer() {
    kfree(m_back_buffer);
}

u8* Framebuffer::write_buffer() const {
    return m_back_buffer;
}

void Framebuffer::swap_buffers() {
    memcpy(m_front_buffer, m_back_buffer, m_buffer_size);
}

void Framebuffer::clear() {
    memset(m_back_buffer, 0, m_buffer_size);
}

u32 Framebuffer::width() const { return m_width; }
u32 Framebuffer::height() const { return m_height; }
u32 Framebuffer::pitch() const { return m_pitch; }
u32 Framebuffer::depth() const { return m_depth; }
u32 Framebuffer::size() const { return m_buffer_size; }

}