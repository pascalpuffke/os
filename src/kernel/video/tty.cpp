#include <kernel/io/serial.h>
#include <kernel/video/tty.h>
#include <kernel/video/vga.h>

namespace Kernel::TTY {

static constexpr auto video_memory_addr = 0xB8000;

static bool initialized = false;

usize current_row;
usize current_column;
u8 current_color;
u16* buffer;

bool is_initialized() {
    return initialized;
}

void initialize() {
    current_row = 0;
    current_column = 0;
    buffer = reinterpret_cast<u16*>(video_memory_addr);
    reset_color();
    for (usize y = 0; y < VGA::height; y++) {
        for (usize x = 0; x < VGA::width; x++) {
            const auto index = y * VGA::width + x;
            buffer[index] = VGA::entry(' ', current_color);
        }
    }
    Cursor::enable(0x0F, 0x0F);
    initialized = true;
}

void scroll() {
    for (usize i = 0; i < VGA::height; ++i) {
        for (usize j = 0; j < VGA::width; ++j) {
            const auto index = i * VGA::width + j;
            if (i == VGA::height - 1)
                buffer[index] = 0;
            else
                buffer[index] = buffer[(i + 1) * VGA::width + j];
        }
    }
}

void set_color(VGA::Color fg, VGA::Color bg) {
    current_color = VGA::entry_color(fg, bg);
}

void reset_color() {
    current_color = VGA::entry_color(VGA::Color::LIGHT_GREEN, VGA::Color::BLACK);
}

int put_entry_at(char c, u8 color, usize x, usize y) {
    const auto index = y * VGA::width + x;
    buffer[index] = VGA::entry(c, color);
    return 1;
}

int put_char(char c) {
    if (IO::Serial::ready())
        IO::Serial::write(static_cast<u8>(c));

    switch (c) {
    case '\n': {
        current_column = 0;
        if (++current_row == VGA::height) {
            scroll();
            --current_row;
        }
        Cursor::move(0, current_row);
    } break;
    case '\t': {
        current_column += 4;
        Cursor::move(current_column, current_row);
    } break;
    case '\b': {
        if (current_column > 0)
            current_column--;
        Cursor::move(current_column, current_row);
    } break;
    default: {
        put_entry_at(c, current_color, current_column, current_row);
        if (++current_column == VGA::width) {
            current_column = 0;
            if (++current_row == VGA::height)
                current_row = 0;
        }
        Cursor::move(current_column, current_row);
    }
    }
    return 1;
}

namespace Cursor {

    void enable(u8 start, u8 end) {
        IO::outb(0x3D4, 0x0A);
        IO::outb(0x3D5, (IO::inb(0x3D5) & 0xC0) | start);

        IO::outb(0x3D4, 0x0B);
        IO::outb(0x3D5, (IO::inb(0x3D5) & 0xE0) | end);
    }

    [[maybe_unused]] void disable() {
        IO::outb(0x3D4, 0x0A);
        IO::outb(0x3D5, 0x20);
    }

    void move(usize x, usize y) {
        const auto position = y * VGA::width + x;
        IO::outb(0x3D4, 0x0F);
        IO::outb(0x3D5, static_cast<u8>(position & 0xFF));
        IO::outb(0x3D4, 0x0E);
        IO::outb(0x3D5, static_cast<u8>((position >> 8) & 0xFF));
    }

    [[maybe_unused]] u16 position() {
        u16 result = 0;
        IO::outb(0x3D4, 0x0F);
        result |= IO::inb(0x3D5);
        IO::outb(0x3D4, 0x0E);
        result |= (static_cast<u16>(IO::inb(0x3D5)) << 8);
        return result;
    }

}

}
