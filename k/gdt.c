#include "include/k/gdt.h"
#include "pretty_printer.h"

GDT_ENTRY gdt_entries[NB_OF_GDT_ENTRIES];
GDT_PTR gdt_first = { sizeof(gdt_entries) - 1, gdt_entries};


void gdt_set_entry(int segment, u32 base_low, u32 limit,
                        u8 access, u8 granularity)
{
    GDT_ENTRY *entry = &(gdt_entries[segment]);
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

void init_GDT()
{
    // TODO: Change magic numbers of gdt_entry
    gdt_set_entry(NULL_SEGMENT, 0, 0, 0, 0);
    gdt_set_entry(KERNEL_CODE_SEGMENT, 0, 0xFFFFF, 0x9A, 0xCF);
    gdt_set_entry(KERNEL_DATA_SEGMENT, 0, 0xFFFFF, 0x92, 0xCF);
    gdt_set_entry(USER_CODE_SEGMENT, 0, 0xFFFFF, 0xFA, 0xCF);
    gdt_set_entry(USER_DATA_SEGMENT, 0, 0xFFFFF, 0xF2, 0xCF);

    // Load GDT by calling load_gdt function
    load_gdt(&gdt_first, KERNEL_CODE_SEGMENT_OFFSET, KERNEL_DATA_SEGMENT_OFFSET, USER_CODE_SEGMENT_OFFSET, USER_DATA_SEGMENT_OFFSET);
}
