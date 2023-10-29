#include "io.h"
#include "include/k/pic_8259.h"

void init_pic_8259(void)
{
    u8 a1, a2;

    /* Save masks registers */
    a1 = inb(PIC1_DATA);
    a2 = inb(PIC2_DATA);

    /* ICW1: Start the initialization sequence (in cascade mode) */
    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4); // 0x11
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4); // 0x11

    /* ICW2: Set vector offset of all default IRQ */
    outb(PIC1_DATA, 0x20);  // The master PIC's offset start from 32 to 39
    outb(PIC2_DATA, 0x28);  // The slave PIC's offset start from 40 to 47

    /* ICW3: Tell Master PIC that there is a slave PIC at IRQ2 (0000 0100) */
    outb(PIC1_DATA, 0x04);
    outb(PIC2_DATA, 0x02);

    /* ICW4: Set x86 mode */
    outb(PIC1_DATA, ICW4_8086);
    outb(PIC2_DATA, ICW4_8086);

    /* Restore masks registers */
    outb(PIC1_DATA, a1);
    outb(PIC2_DATA, a2);
    /* Mask all interrupt */
    // outb(PIC1_DATA, 0x0);
    // outb(PIC2_DATA, 0x0);
}

void send_pic_eoi(u8 irq)
{
    if (irq >= 0x28)
        outb(PIC2_COMMAND, PIC_EOI);
    outb(PIC1_COMMAND, PIC_EOI);
}


void irq_set_mask(u8 irq)
{
    u16 port;
    u8 value;

    if (irq < 0x28)
    {
        port = PIC1_DATA;
    }
    else
    {
        port = PIC2_DATA;
        irq -= 0x28;
    }
    value = inb(port) | (1 << irq);
    outb(port, value);
}

void irq_clear_mask(u8 irq)
{
    u16 port;
    u8 value;

    if (irq < 0x28)
    {
        port = PIC1_DATA;
    }
    else
    {
        port = PIC2_DATA;
        irq -= 0x28;
    }
    value = inb(port) & ~(1 << irq);
    outb(port, value);
}

