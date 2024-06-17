#define KERNEL

#include <kernel/heap/kmalloc.h>
#include <kernel/io/serial.h>
#include <kernel/multiboot/mb.h>
#include <kernel/processor/cpuid.h>
#include <kernel/time/rtc.h>
#include <kernel/util/asm.h>
#include <kernel/util/kassert.h>
#include <kernel/util/kprintf.h>
#include <kernel/video/tty.h>
#include <kernel/video/vbe.h>
#include <kernel/video/vga.h>

#include <libc/string.h>

#include <stdlib/string.h>
#include <stdlib/string_view.h>

#if defined(__linux__)
#error "Not using cross-compiler"
#endif

#if !defined(__i386__)
#error "Kernel needs to be compiled with a 32-bit x86 compiler"
#endif

extern "C" u8 end_of_kernel_image[]; // Set by linker

usize total_system_memory = 0;

namespace Kernel {

struct MemoryMapEntry {
    usize address;
    usize length;
};

MemoryMapEntry find_best_memory(const multiboot_info_t* mbd) {
    usize address = 0;
    usize length = 0;

    for (usize i = 0; i < mbd->mmap_length; i += sizeof(multiboot_memory_map_t)) {
        auto* mmap = reinterpret_cast<multiboot_memory_map_t*>(mbd->mmap_addr + i);

        if (mmap->type != MULTIBOOT_MEMORY_AVAILABLE)
            continue;

        kprintf("Available memory: ");
        TTY::set_color(VGA::Color::LIGHT_GREY, VGA::Color::BLACK);
        // For some stupid reason these two have to be separated, or else
        // just one printf will always display 0x0 as the address.
        kprintf("%dK ", mmap->len / KiB);
        kprintln("at %p", mmap->addr);
        TTY::reset_color();

        total_system_memory += static_cast<usize>(mmap->len);

        // Use whatever is the biggest memory region as the extended memory.
        // Normally we have two usable regions: 640K at 0x0, and everything beyond at 0x100000.
        if (mmap->len > length) {
            address = mmap->addr;
            length = mmap->len;
        }
    }

    return { address, length };
}

void initialize_memory_manager(u64 extended_memory_start, u64 extended_memory_size) {
    // The kernel is loaded 1 MiB in at 0x100000 (see linker.ld)
    // To avoid writing over it, we'll start allocating memory at the end of the kernel image.
    const auto offset = 0x4000;
    const auto kernel_size = reinterpret_cast<usize>(end_of_kernel_image) - extended_memory_start;
    const auto adjusted_memory_size = extended_memory_size - kernel_size - offset;
    MemoryManager::get().initialize(reinterpret_cast<usize>(end_of_kernel_image) + kernel_size + offset, adjusted_memory_size);
}

void print_rtc() {
    auto time = Time::RTC::now();
    kprintf("RTC: ");
    TTY::set_color(VGA::Color::LIGHT_GREY, VGA::Color::BLACK);
    kprintln("%d/%d/%d at %d:%d:%d", time.day, time.month, time.year, time.hour, time.minute, time.second);
    TTY::reset_color();
}

void print_cpu_info(CPUID& cpuid) {
    auto vendor = cpuid.vendor();
    const auto info = cpuid.info();
    kprintf("CPU: ");
    TTY::set_color(VGA::Color::LIGHT_GREY, VGA::Color::BLACK);
    kprintln("%s family %d model %d stepping %d type %d", vendor.data(), info.family, info.model, info.stepping, info.type);
    TTY::reset_color();

    kprintf("CPU features: ");
    TTY::set_color(VGA::Color::LIGHT_GREY, VGA::Color::BLACK);
    for (usize i = 0; i < static_cast<usize>(CPUFeature::_LAST); i++) {
        const auto feature = static_cast<CPUFeature>(i);
        if (cpuid.has_feature(feature))
            kprintf("%s ", cpu_feature_to_string(feature).data());
    }
    TTY::reset_color();
    kputchar('\n');
}

bool test_vbe(const Multiboot& multiboot) {
    auto maybe_vbe = multiboot.vbe();
    if (!maybe_vbe)
        return false;

    const auto vbe = maybe_vbe.release();
    const auto control_info = VBE::read_vbe_info(vbe.control_info);

    if (!strcmp(control_info.signature, "VESA") == 0) {
        panic("invalid VBE control info signature");
    }
    kprintln("VBE present. Version %x", control_info.version);

    return true;
}

Optional<MultibootFramebuffer> test_framebuffer(const Multiboot& multiboot) {
    auto maybe_fb = multiboot.framebuffer();
    if (!maybe_fb)
        return Optional<MultibootFramebuffer>::empty();
    const auto fb = maybe_fb.release();

    return fb;
}

constexpr auto min(auto a, auto b) {
    return a < b ? a : b;
}

}

extern "C" [[gnu::used]] void kernel_main(multiboot_info_t* mbd, u32 magic) {
    using namespace Kernel;

    const auto multiboot = Multiboot { mbd };
    // Neither the TTY nor the Serial devices are set up yet, so should these fail,
    // we will silently end up in Kernel::hang().
    kassert(magic == MULTIBOOT_BOOTLOADER_MAGIC);
    kassert(multiboot.has_flag(MultibootFlag::MEMORY_MAP));

    TTY::initialize();
    if (const auto serial = IO::Serial::initialize(); serial.has_error())
        kprintln("Failed to initialize serial: %s", serial.error().message());

    kprintln("Yeah, this is big brain time.");
    print_rtc();

    const auto memory_map_entry = find_best_memory(mbd);
    kprintf("End of kernel: ");
    TTY::set_color(VGA::Color::LIGHT_GREY, VGA::Color::BLACK);
    kprintln("%p (size: %dK)", end_of_kernel_image, ((usize)end_of_kernel_image - memory_map_entry.address) / KiB);
    TTY::reset_color();
    kprintf("Command line: ");
    TTY::set_color(VGA::Color::LIGHT_GREY, VGA::Color::BLACK);
    kprintln("%s", multiboot.cmdline().value_or("(empty)"));
    TTY::reset_color();
    kprintf("Boot loader: ");
    TTY::set_color(VGA::Color::LIGHT_GREY, VGA::Color::BLACK);
    kprintln("%s", multiboot.boot_loader_name().value_or("(no name)"));
    TTY::reset_color();

    if (!test_vbe(multiboot)) {
        panic("VESA BIOS Extensions (VBE) not present");
    }
    if (auto maybe_fb = test_framebuffer(multiboot); maybe_fb) {
        const auto fb = maybe_fb.release();

        kprintf("Framebuffer present @ 0x%X. ", fb.address);
        kprintf("%dx%d px, ", fb.width, fb.height);
        kprintf("%d bpp, ", fb.depth);
        kprintln("type '%s' (%d)", framebuffer_type_to_string(fb.type), fb.type);

        switch (fb.type) {
        case MultibootFramebufferType::EGA_TEXT:
            break;
        case MultibootFramebufferType::INDEXED:
            break;
        case MultibootFramebufferType::RGB: {
            auto* framebuffer = reinterpret_cast<u8*>(fb.address);
            const auto put_pixel = [&](u32 x, u32 y, u32 color) {
                u32 location = x * 3 + y * fb.pitch;
                framebuffer[location] = color & 0xFF;
                framebuffer[location + 1] = (color >> 8) & 0xFF;
                framebuffer[location + 2] = (color >> 16) & 0xFF;
            };

            for (u32 x = 0; x < fb.width; x++) {
                for (u32 y = 0; y < fb.height; y++) {
                    const u8 r = static_cast<u8>(min(x, 0xFF));
                    const u8 g = static_cast<u8>(min(y, 0xFF));
                    const u8 b = 0xFF;
                    put_pixel(x, y, (r << 16 | g << 8 | b));
                }
            }
        } break;
        }
    }

    initialize_memory_manager(memory_map_entry.address, memory_map_entry.length);

    auto cpuid = CPUID();
    print_cpu_info(cpuid);

    {
        usize count = 0;
        while (true) {
            IO::wait_for(640000);
            ++count;
            kprintln("second %d?", count);
            if (count == 5)
                break;
        }
    }
}
