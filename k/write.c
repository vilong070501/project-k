#include <stddef.h>

#include "io.h"
#include "write.h"

void init_serial()
{
    // Disable all interrupts
    outb(PORT + 1, 0x00);

    // Enable DLAB (set baud rate divisor)
    outb(PORT + 3, 0x80);

    // Set divisor to 3 (lo byte) 38400 baud
    outb(PORT + 0, 0x03);    
    outb(PORT + 1, 0x00);

    // 8 bits, no parity, one stop bit
    outb(PORT + 3, 0x03);

    // Enable FIFO, clear them, with 14-byte threshold
    outb(PORT + 2, 0xC7);    
}

int write(const char *buf, size_t count)
{
    for (size_t i = 0; i < count; i++)
    {
        outb(PORT, buf[i]);
    }
    return count;
}
