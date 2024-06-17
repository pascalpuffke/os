#pragma once

#include <kernel/multiboot/mb.h>

namespace Kernel {

class Framebuffer final {
public:
    Framebuffer(u8* hw_buffer, u32 width, u32 height, u32 pitch, u32 depth);
    ~Framebuffer();

    /**
     * @brief Get a framebuffer suitable for writing to. Changes are not visible to the user until
     *        `swap_buffers()` is called.
     * 
     * @see swap_buffers()
     */
    [[nodiscard]] u8* write_buffer() const;
    /**
     * @brief Swap front and back buffers and copy their contents to hardware.
     */
    void swap_buffers();

    void clear();

    [[nodiscard]] u32 width() const;
    [[nodiscard]] u32 height() const;
    [[nodiscard]] u32 pitch() const;
    [[nodiscard]] u32 depth() const;
    [[nodiscard]] u32 size() const;

private:
    u32 m_width;
    u32 m_height;
    u32 m_pitch;
    u32 m_depth;
    u32 m_buffer_size;

    u8* m_front_buffer;
    u8* m_back_buffer;
};

}