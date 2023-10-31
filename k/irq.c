#include "include/k/irq.h"
#include "include/k/isr.h"
#include "include/k/pic_8259.h"
#include "include/k/types.h"
#include "../libs/libc/include/stdio.h"
#include "io.h"


#define PIC_REMAP_OFFSET 0x20

IRQHandler IRQ_handlersMap[16];

extern void enable_interrupts();

void IRQ_handler(Registers *regs)
{
    int irq = regs->int_no - PIC_REMAP_OFFSET;
    
    u8 pic_isr = readInServiceRegister();
    u8 pic_irr = readIRQRequestRegister();

    if (IRQ_handlersMap[irq] != NULL)
    {
        // Handle IRQ
        IRQ_handlersMap[irq](regs);
    }
    else
    {
        printf("Unhandled IRQ %d ISR=%x IRR=%x...\n", irq, pic_isr, pic_irr);
    }

    // Send EOI
    send_PIC_EOI(irq);
}

void IRQ_RegisterHandler(int irq, IRQHandler handler)
{
    IRQ_handlersMap[irq] = handler;
}

void init_IRQ(void)
{
    init_PIC_8259(PIC_REMAP_OFFSET, PIC_REMAP_OFFSET + 8);

    /* Register ISR handlers for each of the 16 irq lines */
    for (int i = 0; i < 16; i++)
    {
        ISR_RegisterHandler(PIC_REMAP_OFFSET + i, IRQ_handler);
    }

    /* Enable interrupts */
    enable_interrupts();
}