#include "include/k/console.h"
#include "include/k/gdt.h"
#include "include/k/tss.h"
#include "../libs/libc/include/stdio.h"
#include "../libs/libc/include/string.h"

TSS default_tss;

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
    default_tss.io_map = (u16)sizeof(TSS);
}

void init_TSS()
{
    set_tss_entry(0x10, 0x0);
}

void set_tss_stack(u32 esp0)
{
    default_tss.esp0 = esp0;
}
