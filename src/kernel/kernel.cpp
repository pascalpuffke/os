#define KERNEL

#include <kernel/heap/kmalloc.h>
#include <kernel/io/serial.h>
#include <kernel/multiboot.h>
#include <kernel/processor/cpuid.h>
#include <kernel/time/rtc.h>
#include <kernel/util/asm.h>
#include <kernel/util/kassert.h>
#include <kernel/util/kprintf.h>
#include <kernel/video/tty.h>
#include <kernel/video/vga.h>

#if defined(__linux__)
#error "Not using cross-compiler"
#endif

#if !defined(__i386__)
#error "Kernel needs to be compiled with a 32-bit x86 compiler"
#endif

#define KMUST(expr) ({                                                                                        \
    auto result = (expr);                                                                                     \
    if (!result) [[unlikely]] {                                                                               \
        Kernel::TTY::set_color(Kernel::VGA::Color::LIGHT_RED, Kernel::VGA::Color::BLACK);                     \
        kprintln("ASSERTION FAILED: %s\n      expression: %s\n         in file: %s:%d\n        function: %s", \
            result.error().message(), #expr, __FILE__, __LINE__, __PRETTY_FUNCTION__);                        \
        Kernel::hang();                                                                                       \
    }                                                                                                         \
    result;                                                                                                   \
})

extern "C" u8 end_of_kernel_image[]; // Set by linker

usize total_system_memory = 0;

namespace Kernel {

struct MemoryMapEntry {
    u64 address;
    u64 length;
};

MemoryMapEntry find_best_memory(const multiboot_info_t* mbd)
{
    u64 address = 0;
    u64 length = 0;

    for (usize i = 0; i < mbd->mmap_length; i += sizeof(multiboot_memory_map_t)) {
        auto* mmap = reinterpret_cast<multiboot_memory_map_t*>(mbd->mmap_addr + i);

        if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE) {
            kprintf("Available memory: ");
            TTY::set_color(VGA::Color::LIGHT_GREY, VGA::Color::BLACK);
            // For some stupid reason these two have to be separated, or else
            // just one printf will always display 0x0 as the address.
            kprintf("%dK ", mmap->len / KiB);
            kprintln("at 0x%X", mmap->addr);
            TTY::reset_color();

            total_system_memory += mmap->len;

            // Use whatever is the biggest memory region as the extended memory.
            // Normally we have two usable regions: 640K at 0x0, and everything beyond at 0x100000.
            if (mmap->len > length) {
                address = mmap->addr;
                length = mmap->len;
            }
        }
    }

    return { address, length };
}

void initialize_memory_manager(u64 extended_memory_start, u64 extended_memory_size)
{
    // The kernel is loaded 1 MiB in at 0x100000 (see linker.ld)
    // To avoid writing over it, we'll start allocating memory at the end of the kernel image.
    const auto offset = 0x4000;
    const auto kernel_size = (usize)end_of_kernel_image - extended_memory_start;
    const auto adjusted_memory_size = extended_memory_size - kernel_size - offset;
    MemoryManager::get().initialize((usize)end_of_kernel_image + kernel_size + offset, adjusted_memory_size);
}

void print_rtc()
{
    auto time = Time::RTC::now();
    kprintf("RTC: ");
    TTY::set_color(VGA::Color::LIGHT_GREY, VGA::Color::BLACK);
    kprintln("%d/%d/%d at %d:%d:%d", time.day, time.month, time.year, time.hour, time.minute, time.second);
    TTY::reset_color();
}

void print_cpu_info(CPUID& cpuid)
{
    const auto* vendor = cpuid.vendor();
    const auto info = cpuid.info();
    kprintf("CPU: ");
    TTY::set_color(VGA::Color::LIGHT_GREY, VGA::Color::BLACK);
    kprintln("%s family %d model %d stepping %d type %d", vendor, info.family, info.model, info.stepping, info.type);
    TTY::reset_color();

    kprintf("CPU features: ");
    TTY::set_color(VGA::Color::LIGHT_GREY, VGA::Color::BLACK);
    for (usize i = 0; i < (usize)CPUFeature::_LAST; i++) {
        auto feature = (CPUFeature)i;
        if (cpuid.has_feature(feature)) {
            kprintf("%s ", cpu_feature_to_string(feature));
        }
    }
    TTY::reset_color();
    kputchar('\n');
}

}

extern "C" void kernel_main(multiboot_info_t* mbd, unsigned int magic)
{
    using namespace Kernel;

    TTY::initialize();
    KMUST(IO::Serial::initialize());

    // Check these after initializing the TTY and serial port, as otherwise
    // we wouldn't even know the assertion failed.
    kassert(magic == MULTIBOOT_BOOTLOADER_MAGIC);
    kassert(mbd->flags >> 6 & 1);

    kprintln("Yeah, this is big brain time.");
    print_rtc();

    auto cpuid = CPUID();
    print_cpu_info(cpuid);

    const auto memory_map_entry = find_best_memory(mbd);
    kprintf("End of kernel: ");
    TTY::set_color(VGA::Color::LIGHT_GREY, VGA::Color::BLACK);
    kprintln("0x%X (size: %dK)", end_of_kernel_image, ((usize)end_of_kernel_image - memory_map_entry.address) / KiB);
    TTY::reset_color();

    initialize_memory_manager(memory_map_entry.address, memory_map_entry.length);
}
