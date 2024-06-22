#define KERNEL

#include <kernel/fs/vfs.h>
#include <kernel/heap/kmalloc.h>
#include <kernel/interrupts/gdt.h>
#include <kernel/interrupts/pit.h>
#include <kernel/io/serial.h>
#include <kernel/processor/cpuid.h>
#include <kernel/time/rtc.h>
#include <kernel/util/asm.h>
#include <kernel/util/kassert.h>
#include <kernel/util/kprintf.h>
#include <kernel/video/fb.h>
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

extern "C" {
extern const u8 _binary_bad_apple_bin_start[];
extern const u8 _binary_bad_apple_bin_end[];
extern const u8 _binary_bad_apple_bin_size;
}

namespace Kernel {

u64 total_system_memory = 0;

Framebuffer* framebuffer;

struct MemoryMapEntry {
    u64 address;
    u64 length;
};

MemoryMapEntry find_best_memory(const multiboot_info_t* mbd) {
    u64 address = 0;
    u64 length = 0;

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

    if (!strcmp(control_info.signature, "VESA") == 0)
        panic("invalid VBE control info signature");
    kprintln("VBE present. Version %x", control_info.version);

    return true;
}

Framebuffer* setup_framebuffer(const Multiboot& multiboot) {
    auto maybe_fb = multiboot.framebuffer();
    if (!maybe_fb)
        return nullptr;
    const auto mb_fb = maybe_fb.release();

    const auto type = mb_fb.type;
    const auto width = mb_fb.width;
    const auto height = mb_fb.height;
    const auto depth = mb_fb.depth;
    const auto pitch = mb_fb.pitch;
    u8* address = reinterpret_cast<u8*>(mb_fb.address);

    kprintf("Framebuffer present @ %p. ", address);
    kprintf("%dx%d px, ", width, height);
    kprintf("%d bpp, %d pitch, ", depth, pitch);
    kprintln("type '%s' (%d)", framebuffer_type_to_string(type), type);

    if (type != MultibootFramebufferType::RGB)
        panic("Unsupported multiboot framebuffer type");

    return new Framebuffer(address, width, height, pitch, depth);
}

void play_the_funny() {
    static constexpr u32 width = 64;
    static constexpr u32 height = 48;
    static constexpr u32 n_frames = 6570;

    kassert(width < framebuffer->width());
    kassert(height < framebuffer->height());

    const u8* data_start = _binary_bad_apple_bin_start;
    usize data_size = _binary_bad_apple_bin_end - _binary_bad_apple_bin_start;
    usize size_per_frame = width * height;

    const auto put_pixel = [](u8* buffer, u32 x, u32 y, u8 color, u32 pitch) {
        u32 location = x * 3 + y * pitch;
        buffer[location] = color;
        buffer[location + 1] = color;
        buffer[location + 2] = color;

        buffer[location + 3] = color;
        buffer[location + 4] = color;
        buffer[location + 5] = color;

        location += pitch;
        buffer[location] = color;
        buffer[location + 1] = color;
        buffer[location + 2] = color;

        buffer[location + 3] = color;
        buffer[location + 4] = color;
        buffer[location + 5] = color;
    };

    const auto pitch = framebuffer->pitch();

    for (usize frame = 0; frame < n_frames; frame++) {
        framebuffer->clear();

        auto* write_buffer = framebuffer->write_buffer();

        if (frame * size_per_frame >= data_size)
            break;

        for (usize x = 0; x < width; x++) {
            for (usize y = 0; y < height; y++) {
                usize scaled_x = x * 2;
                usize scaled_y = y * 2;

                u8 pixel = data_start[frame * size_per_frame + (y * width + x)];

                put_pixel(write_buffer, scaled_x, scaled_y, pixel, pitch);
                put_pixel(write_buffer, scaled_x + 1, scaled_y, pixel, pitch);
                put_pixel(write_buffer, scaled_x, scaled_y + 1, pixel, pitch);
                put_pixel(write_buffer, scaled_x + 1, scaled_y + 1, pixel, pitch);
            }
        }

        framebuffer->swap_buffers();

        IO::wait_for(10000);
    }
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

    if (!test_vbe(multiboot))
        panic("VESA BIOS Extensions (VBE) not present");

    initialize_memory_manager(memory_map_entry.address, memory_map_entry.length);
    framebuffer = setup_framebuffer(multiboot);

    auto cpuid = CPUID();
    print_cpu_info(cpuid);

    play_the_funny();

    delete framebuffer;
}
