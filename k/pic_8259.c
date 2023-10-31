#include "include/k/pic_8259.h"
#include "include/k/types.h"
#include "io.h"

void init_PIC_8259(u8 offsetPIC1, u8 offsetPIC2)
{

    /* ICW1: Start the initialization sequence (in cascade mode) */
    outb(PIC1_COMMAND_PORT, ICW1_INIT | ICW1_ICW4); // 0x11
    iowait();
    outb(PIC2_COMMAND_PORT, ICW1_INIT | ICW1_ICW4); // 0x11
    iowait();

    /* ICW2: Set vector offset of all default IRQs */
    outb(PIC1_DATA_PORT, offsetPIC1);  // The master PIC's offset start from 32 to 39
    iowait();
    outb(PIC2_DATA_PORT, offsetPIC2);  // The slave PIC's offset start from 40 to 47
    iowait();

    /* ICW3: Tell Master PIC that there is a slave PIC at IRQ2 (0000 0100) */
    outb(PIC1_DATA_PORT, 0x04); // Tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
    iowait();
    outb(PIC2_DATA_PORT, 0x02); // Tell Slave PIC its cascade identity
    iowait();

    /* ICW4: Set x86 mode */
    outb(PIC1_DATA_PORT, ICW4_8086);
    iowait();
    outb(PIC2_DATA_PORT, ICW4_8086);
    iowait();

    /* Mask all interrupt */
    outb(PIC1_DATA_PORT, 0x0);
    iowait();
    outb(PIC1_DATA_PORT, 0x0);
    iowait();
}

void IRQ_set_mask(int irq)
{
    u16 port;
    u8 value;

    if (irq < 8)
    {
        port = PIC1_DATA_PORT;
    }
    else
    {
        irq -= 8;
        port = PIC2_DATA_PORT;
        
    }
    value = inb(port) | (1 << irq);
    outb(port, value);
}

void IRQ_clear_mask(int irq)
{
    u16 port;
    u8 value;

    if (irq < 8)
    {
        port = PIC1_DATA_PORT;
    }
    else
    {
        irq -= 8;
        port = PIC2_DATA_PORT;
    }
    value = inb(port) & ~(1 << irq);
    outb(port, value);
}

void disable_PIC(void)
{
    outb(PIC1_DATA_PORT, 0xFF);
    iowait();
    outb(PIC2_DATA_PORT, 0xFF);
    iowait();
}

void send_PIC_EOI(int irq)
{
    if (irq >= 8)
        outb(PIC2_COMMAND_PORT, PIC_EOI);
    outb(PIC1_COMMAND_PORT, PIC_EOI);
}

u16 readIRQRequestRegister(void)
{
    outb(PIC1_COMMAND_PORT, PIC_READ_IRR);
    outb(PIC2_COMMAND_PORT, PIC_READ_IRR);
    return ((u16)inb(PIC2_COMMAND_PORT)) | (((u16)inb(PIC2_COMMAND_PORT)) << 8);
}

u16 readInServiceRegister(void)
{
    outb(PIC1_COMMAND_PORT, PIC_READ_ISR);
    outb(PIC2_COMMAND_PORT, PIC_READ_ISR);
    return ((u16)inb(PIC2_COMMAND_PORT)) | (((u16)inb(PIC2_COMMAND_PORT)) << 8);
}