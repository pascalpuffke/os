#include <kernel/io/serial.h>
#include <kernel/video/tty.h>
#include <kernel/video/vga.h>
#include <libc/string.h>

namespace Kernel::TTY {

usize row;
usize column;
u8 color;
u16* buffer;

void reset_color()
{
    color = VGA::entry_color(VGA::Color::LIGHT_GREEN, VGA::Color::BLACK);
}

void initialize()
{
    row = 0;
    column = 0;
    buffer = reinterpret_cast<u16*>(0xB8000);
    reset_color();
    for (usize y = 0; y < VGA::height; y++) {
        for (usize x = 0; x < VGA::width; x++) {
            const auto index = y * VGA::width + x;
            buffer[index] = VGA::entry(' ', color);
        }
    }
    Cursor::enable(0x0F, 0x0F);
}

void scroll()
{
    for (usize i = 0; i < VGA::height; ++i) {
        for (usize j = 0; j < VGA::width; ++j) {
            if (i == VGA::height - 1) {
                buffer[i * VGA::width + j] = 0;
            } else {
                buffer[i * VGA::width + j] = buffer[(i + 1) * VGA::width + j];
            }
        }
    }
}

void set_color(VGA::Color fg, VGA::Color bg)
{
    color = VGA::entry_color(fg, bg);
}

int put_entry_at(char c, u8 color, usize x, usize y)
{
    const auto index = y * VGA::width + x;
    buffer[index] = VGA::entry(c, color);
    return 1;
}

int put_char(char c)
{
    if (IO::Serial::ready())
        IO::Serial::write(c);
    switch (c) {
    case '\n': {
        column = 0;
        if (++row == VGA::height) {
            scroll();
            --row;
        }
        Cursor::move(0, row);
    } break;
    case '\t': {
        column += 4;
        Cursor::move(column, row);
    } break;
    case '\b': {
        if (column > 0)
            column--;
        Cursor::move(column, row);
    } break;
    default: {
        put_entry_at(c, color, column, row);
        if (++column == VGA::width) {
            column = 0;
            if (++row == VGA::height)
                row = 0;
        }
        Cursor::move(column, row);
    }
    }
    return 1;
}

int write(const char* data, usize size)
{
    int written = 0;
    for (usize i = 0; i < size; ++i)
        written += put_char(data[i]);
    return written;
}

int write_string(const char* data)
{
    return write(data, strlen(data));
}

namespace Cursor {

    void enable(u8 start, u8 end)
    {
        IO::outb(0x3D4, 0x0A);
        IO::outb(0x3D5, (IO::inb(0x3D5) & 0xC0) | start);

        IO::outb(0x3D4, 0x0B);
        IO::outb(0x3D5, (IO::inb(0x3D5) & 0xE0) | end);
    }

    void disable()
    {
        IO::outb(0x3D4, 0x0A);
        IO::outb(0x3D5, 0x20);
    }

    void move(usize x, usize y)
    {
        const auto position = y * VGA::width + x;
        IO::outb(0x3D4, 0x0F);
        IO::outb(0x3D5, static_cast<u8>(position & 0xFF));
        IO::outb(0x3D4, 0x0E);
        IO::outb(0x3D5, static_cast<u8>((position >> 8) & 0xFF));
    }

    u16 position()
    {
        u16 result = 0;
        IO::outb(0x3D4, 0x0F);
        result |= IO::inb(0x3D5);
        IO::outb(0x3D4, 0x0E);
        result |= (static_cast<u16>(IO::inb(0x3D5)) << 8);
        return result;
    }

}

}
