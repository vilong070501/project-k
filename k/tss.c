#include "include/k/console.h"
#include "include/k/gdt.h"
#include "include/k/tss.h"
#include "../libs/libc/include/stdio.h"
#include "../libs/libc/include/string.h"

TSS default_tss;

void load_tss(void)
{
    asm volatile ("mov $0x2B, %ax\n");
    asm volatile ("ltr %ax\n");
    asm volatile ("ret\n");
}

static void set_tss_entry(u16 ss0, u32 esp0)
{
    u32 base = (u32) &default_tss;
    u32 limit = base + sizeof(default_tss);

    gdt_set_entry(TSS_SEGMENT, base, limit, 0xE9, 0x00);
    memset(&default_tss, 0x0, sizeof(TSS));

    default_tss.ss0 = ss0;
    default_tss.esp0 = esp0;
    default_tss.cs = 0x0b;
    default_tss.ss = 0x13;
    default_tss.ds = 0x13;
    default_tss.es = 0x13;
    default_tss.fs = 0x13;
    default_tss.gs = 0x13;
    default_tss.iomap_base = (u16)sizeof(TSS);
}

void init_TSS(void)
{
    set_tss_entry(0x10, 0x0);
}

void set_tss_stack(u32 esp0)
{
    default_tss.esp0 = esp0;
}

void print_tss(void) {
    printf("previous: 0x%x\n", default_tss.previous_task);
    printf("esp0: 0x%x, ss0: 0x%x\n", default_tss.esp0, default_tss.ss0);
    printf("esp1: 0x%x, ss1: 0x%x\n", default_tss.esp1, default_tss.ss1);
    printf("esp2: 0x%x, ss2: 0x%x\n", default_tss.esp2, default_tss.ss2);
    printf("cr3: 0x%x, eip: 0x%x, eflags: 0x%x\n", default_tss.cr3, default_tss.eip, default_tss.eflags);
    printf("eax: 0x%x, ecx: 0x%x, edx: 0x%x, ebx: 0x%x\n", default_tss.eax, default_tss.ecx, default_tss.edx, default_tss.ebx);
    printf("esp: 0x%x, ebp: 0x%x, esi: 0x%x, edi: 0x%x\n", default_tss.esp, default_tss.ebp, default_tss.esi, default_tss.edi);
    printf("es:0x%x, cs:0x%x, ss:0x%x, ds:0x%x, fs:0x%x, gs:0x%x\n", default_tss.es, default_tss.cs, default_tss.ss, default_tss.ds, default_tss.fs, default_tss.gs);
    printf("ldt: 0x%x, trap: 0x%x, iomap_base: 0x%x\n", default_tss.iomap_base, default_tss.trap, default_tss.iomap_base);
}