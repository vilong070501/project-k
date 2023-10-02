#ifndef LOAD_GDT_H
#define LOAD_GDT_H

#include "gdt.h"

static inline void load_gdt(struct GDT *gdt_entries)
{
    asm volatile ("lgdt %0" : /* No output */ : "m"(gdt_entries) : "memory");

    asm volatile (" movl $0x42, %eax\n \
                    movl %eax, %cr0\n");

    asm volatile (" mov 4(%esp), %eax \n \
                    lgdt (%eax)       \n \
                    mov $0x10, %eax   \n \
                    mov %eax, %ds     \n \
                    mov %eax, %es     \n \
                    mov %eax, %fs     \n \
                    mov %eax, %gs     \n \
                    mov %eax, %ss     \n \
                    jmp $0x8, $.long_jump   \n \
                    .long_jump:       \n \
                        ret \n");
}

#endif
