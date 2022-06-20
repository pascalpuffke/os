#include <kernel/heap/kmalloc.h>
#include <kernel/io/serial.h>
#include <kernel/multiboot.h>
#include <kernel/time/rtc.h>
#include <kernel/util/kassert.h>
#include <kernel/util/kprintf.h>
#include <kernel/video/tty.h>
#include <kernel/video/vga.h>
#include <stdlib/result.h>

/*
#if defined(__linux__)
#error "Not using cross-compiler"
#endif

#if !defined(__i386__)
#error "Kernel needs to be compiled with a 32-bit x86 compiler"
#endif
*/

extern "C" void kernel_main(multiboot_info_t* mbd, unsigned int magic)
{
    using namespace Kernel;

    TTY::initialize();
    if (!IO::Serial::initialize()) {
        TTY::set_color(VGA::Color::RED, VGA::Color::BLACK);
        kprintf("Failed to initialize serial port\n");
    }

    // Check these after initializing the TTY and serial port, as otherwise
    // we wouldn't even know the assertion failed.
    kassert(magic == MULTIBOOT_BOOTLOADER_MAGIC);
    kassert(mbd->flags >> 6 & 1);

    kprintf("Yeah, this is big brain time.\n");
    auto time = Time::RTC::now();
    kprintf("RTC: ");
    TTY::set_color(VGA::Color::LIGHT_GREY, VGA::Color::BLACK);
    kprintf("%d/%d/%d at %d:%d:%d\n", time.day, time.month, time.year, time.hour, time.minute, time.second);
    TTY::reset_color();

    void* best_available_memory = nullptr;
    usize best_available_memory_size = 0;
    usize total_system_memory = 0;

    for (usize i = 0; i < mbd->mmap_length; i += sizeof(multiboot_memory_map_t)) {
        auto* mmap = reinterpret_cast<multiboot_memory_map_t*>(mbd->mmap_addr + i);

        if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE) {
            kprintf("Available memory: ");
            TTY::set_color(VGA::Color::LIGHT_GREY, VGA::Color::BLACK);
            // For some stupid reason these two have to be separated, or else
            // just one printf will always display 0x0 as the address.
            kprintf("%dK ", mmap->len / 1024);
            kprintf("at 0x%X\n", mmap->addr);
            TTY::reset_color();
            total_system_memory += mmap->len;
            if (mmap->len > best_available_memory_size) {
                best_available_memory = reinterpret_cast<void*>(mmap->addr);
                best_available_memory_size = mmap->len;
            }
        }
    }

    kprintf("Total system memory: ");
    TTY::set_color(VGA::Color::LIGHT_GREY, VGA::Color::BLACK);
    kprintf("%d MiB\n", total_system_memory / 1024 / 1024);
    TTY::reset_color();
    MemoryManager::the().initialize(best_available_memory, best_available_memory_size);

    {
        Result<int> res = 10;
        auto nice = res
                        .and_then([](int x) { return x * 2; })
                        .and_then([](int x) { return x + 49; })
                        .or_else([](Error e) { kprintf("Error: %s\n", e.message()); return 0; })
                        .value();
        kprintf("Result: %d\n", nice);
    }
}
