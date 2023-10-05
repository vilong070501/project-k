#include "io.h"
#include "include/k/pic_8259.h"

void init_pic_8259()
{
    //u8 a1, a2;

    /* Save masks registers */
    //a1 = inb(PIC1_DATA);
    //a2 = inb(PIC2_DATA);

    /* ICW1: Start the initialization sequence (in cascade mode) */
    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4); // 0x11
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4); // 0x11

    /* ICW2: Set vector offset of all default IRQ */
    outb(PIC1_DATA, 0x40);  // The master PIC's offset start from 64
    outb(PIC2_DATA, 0x50);  // The slave PIC's offset start from 80

    /* ICW3: Tell Master PIC that there is a slave PIC at IRQ2 (0000 0100) */
    outb(PIC1_DATA, 0x04);
    outb(PIC2_DATA, 0x02);

    /* ICW4: Set x86 mode */
    outb(PIC1_DATA, ICW4_8086);
    outb(PIC2_DATA, ICW4_8086);

    /* Restore masks registers */
    //outb(PIC1_DATA, a1);
    //outb(PIC2_DATA, a2);
    /* Mask all interrupt */
    outb(PIC1_DATA, 0x0);
    outb(PIC2_DATA, 0x0);
}

void send_pic_eoi(u8 irq)
{
    if (irq >= 0x50)
        outb(PIC2_COMMAND, PIC_EOI);
    outb(PIC1_COMMAND, PIC_EOI);
}

