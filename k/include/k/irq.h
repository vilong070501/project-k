#ifndef IRQ_H
#define IRQ_H

#include "isr.h"

typedef void (*IRQHandler)(Registers* regs);

void IRQ_RegisterHandler(int irq, IRQHandler handler);
void init_IRQ(void);

#endif
