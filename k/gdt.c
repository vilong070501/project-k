#include "include/k/gdt.h"
#include "pretty_printer.h"

GDT gdt_entries[NB_OF_GDT_ENTRIES];
GDT_PTR gdt_first;


void gdt_set_entry(int segment, u32 base_low, u32 limit,
                        u8 access, u8 granularity)
{
    GDT *entry = &(gdt_entries[segment]);
    /* Setup the descriptor base address */
    entry->base_low = (base_low & 0xFFFF);
    entry->base_middle = (base_low >> 16) & 0xFF;
    entry->base_high = (base_low >> 24) & 0xFF;

    /* Setup the descriptor limits */
    entry->segment_limit = (limit & 0xFFFF);
    entry->granularity = (limit >> 16) & 0x0F;

    /* Setup the granularity and access flag */
    entry->granularity |= (granularity & 0xF0);
    entry->access = access;
}

void init_gdt()
{
    gdt_first.limit_size = sizeof(gdt_entries) - 1;
    gdt_first.base_address = (GDT*)&gdt_entries;

    gdt_set_entry(NULL_SEGMENT, 0, 0, 0, 0);
    gdt_set_entry(KERNEL_CODE_SEGMENT, 0, 0xFFFFF, 0x9A, 0xCF);
    gdt_set_entry(KERNEL_DATA_SEGMENT, 0, 0xFFFFF, 0x92, 0xCF);
    gdt_set_entry(USER_CODE_SEGMENT, 0, 0xFFFFF, 0xFA, 0xCF);
    gdt_set_entry(USER_DATA_SEGMENT, 0, 0xFFFFF, 0xF2, 0xCF);

    GDT* first = (GDT*)&gdt_first;

    // Load GDT by calling load_gdt function
    asm volatile ("lgdt %0" : /* No output */ : "m"(*first) : "memory");
    load_gdt();
    // gdt_pretty_print(&gdt_first);
}
