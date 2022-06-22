#include <kernel/util/asm.h>
#include <kernel/util/kassert.h>
#include <kernel/util/kprintf.h>
#include <kernel/video/tty.h>
#include <kernel/video/vga.h>

void _kassert(const char* file, int line, const char* func, const char* expr)
{
    Kernel::TTY::set_color(Kernel::VGA::Color::LIGHT_RED, Kernel::VGA::Color::BLACK);
    kprintf("ASSERTION FAILED: %s\n         in file: %s:%d\n        function: %s\n", expr, file, line, func);
    Kernel::hang();
}

void _kassert_msg(const char* file, int line, const char* func, const char* expr, const char* format)
{
    Kernel::TTY::set_color(Kernel::VGA::Color::LIGHT_RED, Kernel::VGA::Color::BLACK);
    kprintf("ASSERTION FAILED: %s\n      expression: %s\n         in file: %s:%d\n        function: %s\n", format, expr, file, line, func);
    Kernel::hang();
}
