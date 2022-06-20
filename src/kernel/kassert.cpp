#include <kernel/util/asm.h>
#include <kernel/util/kassert.h>
#include <kernel/util/kprintf.h>
#include <kernel/video/tty.h>
#include <kernel/video/vga.h>

void _kassert(const char* file, int line, const char* func, const char* expr)
{
    Kernel::TTY::set_color(Kernel::VGA::Color::LIGHT_RED, Kernel::VGA::Color::BLACK);
    kprintf("ASSERTION FAILED: %s\n\t in file: %s:%d\n\tfunction: %s\n", expr, file, line, func);
    Kernel::hang();
}

void _kassert_msg(const char* file, int line, const char* func, const char* expr, const char* msg)
{
    Kernel::TTY::set_color(Kernel::VGA::Color::LIGHT_RED, Kernel::VGA::Color::BLACK);
    kprintf("ASSERTION FAILED: %s (%s)\n\t in file: %s:%d\n\tfunction: %s\n", msg, expr, file, line, func);
    Kernel::hang();
}
