#ifndef LOAD_GDT_H
#define LOAD_GDT_H

#include "gdt.h"

static inline void load_gdt(struct GDT *gdt_entries)
{
    asm volatile ("lgdt %0" : /* No output */ : "m"(*gdt_entries) : "memory");

    asm volatile (" movw $0x10, %ax   \n \
                    movw %ax, %ds     \n \
                    movw %ax, %es     \n \
                    movw %ax, %fs     \n \
                    movw %ax, %gs     \n \
                    movw %ax, %ss     \n \
                    ljmp $0x08, $next \n \
                    next:      	      \n");
}

#endif
