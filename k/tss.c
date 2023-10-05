#include "include/k/tss.h"
#include "include/k/gdt.h"
#include "../libs/libc/include/stdio.h"
#include "../libs/libc/include/string.h"

struct TSS default_tss;

extern u32 get_eip()
{
    u32 eip;
    asm volatile ("mov (%%esp), %0" : "=r"(eip));
    return eip;
}

void load_tss()
{
    asm volatile ("mov $0x2B, %ax\n");
    asm volatile ("ltr %ax\n");
}

static void set_tss_entry(u16 ss0, u32 esp0)
{
    u32 eip = get_eip();
    printf("eip = %x\n", eip);
    u32 base = (u32) &default_tss;
    u32 limit = base + sizeof(struct TSS);

    create_descriptor(TSS_SEGMENT, base, limit, 0xE9, 0x00);
    memset(&default_tss, 0x0, sizeof(struct TSS));

    default_tss.ss0 = ss0;
    default_tss.esp0 = esp0;
    default_tss.eip = eip;
    default_tss.cs = 0x0b;
    default_tss.ss = 0x13;
    default_tss.ds = 0x13;
    default_tss.es = 0x13;
    default_tss.fs = 0x13;
    default_tss.gs = 0x13;
    default_tss.io_map = sizeof(struct TSS);
}

void init_tss()
{
    set_tss_entry(0x10, 0x0100000);
    load_tss();
}

void set_tss_stack(u32 esp0)
{
    default_tss.esp0 = esp0;
}
