#ifndef LOAD_IDT_H
#define LOAD_IDT_H

#include "idt.h"

static inline void load_idt(struct IDT *idt_entries)
{
    asm volatile ("lidt %0" : /* No output */ : "m"(*idt_entries) : "memory");
}

#endif
