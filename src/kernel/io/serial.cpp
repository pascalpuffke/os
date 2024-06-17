#include <kernel/io/serial.h>

namespace Kernel::IO::Serial {

static constexpr u16 serial_port = 0x3F8;
static bool initialized = false;

Result<void> initialize() {
    outb(serial_port + 1, 0x00); // Disable all interrupts
    outb(serial_port + 3, 0x80); // Enable DLAB (set baud rate divisor)
    outb(serial_port + 0, 0x03); // Set divisor to 3 (lo byte) 38400 baud
    outb(serial_port + 1, 0x00); //                  (hi byte)
    outb(serial_port + 3, 0x03); // 8 bits, no parity, one stop bit
    outb(serial_port + 2, 0xC7); // Enable FIFO, clear them, with 14-byte threshold
    outb(serial_port + 4, 0x0B); // IRQs enabled, RTS/DSR set
    outb(serial_port + 4, 0x1E); // Set in loopback mode, test the serial chip
    outb(serial_port + 0, 0xAE); // Test serial chip (send byte 0xAE and check if serial returns same byte)

    if (inb(serial_port) != 0xAE)
        return Error { "Faulty serial chip" };

    // Set serial chip to normal mode
    outb(serial_port + 4, 0x0F);
    initialized = true;
    return {};
}

bool ready() { return initialized; }

bool received() { return inb(serial_port + 5) & 1; }

bool is_transmit_empty() { return inb(serial_port + 5) & 0x20; }

u8 read() {
    while (!received())
        ;
    return inb(serial_port);
}

void write(u8 byte) {
    while (!is_transmit_empty())
        ;
    outb(serial_port, byte);
}

void write_string(const char* data, usize length) {
    for (usize i = 0; i < length; i++)
        write(static_cast<u8>(data[i]));
}

}
