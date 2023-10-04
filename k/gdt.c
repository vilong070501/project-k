#include "gdt.h"
#include "load_gdt.h"
#include "pretty_printer.h"

struct GDT gdt_entries[6];
struct GDT_PTR gdt_first;

extern void load_gdt(struct GDT*);

void create_descriptor(int segment, u32 base_low, u32 limit,
                        u8 access, u8 granularity)
{
    /* Setup the descriptor base address */
    gdt_entries[segment].base_low = (base_low & 0xFFFF);
    gdt_entries[segment].base_middle = (base_low >> 16) & 0xFF;
    gdt_entries[segment].base_high = (base_low >> 24) & 0xFF;

    /* Setup the descriptor limits */
    gdt_entries[segment].segment_limit = (limit & 0xFFFF);
    gdt_entries[segment].granularity = (limit >> 16) & 0x0F;

    /* Setup the granularity and access flag */
    gdt_entries[segment].granularity |= (granularity & 0xF0);
    gdt_entries[segment].access = access;
}

void init_gdt()
{
    create_descriptor(NULL_SEGMENT, 0, 0, 0, 0);
    create_descriptor(KERNEL_CODE_SEGMENT, 0, 0xFFFFF, 0x9A, 0xCF);
    create_descriptor(KERNEL_DATA_SEGMENT, 0, 0xFFFFF, 0x92, 0xCF);
    create_descriptor(USER_CODE_SEGMENT, 0, 0xFFFFF, 0xFA, 0xCF);
    create_descriptor(USER_DATA_SEGMENT, 0, 0xFFFFF, 0xF2, 0xCF);

    gdt_first.limit_size = sizeof(gdt_entries) - 1;
    gdt_first.base_address = (struct GDT*)&gdt_entries;

    // Load GDT by calling load_gdt function
    load_gdt((struct GDT*)&gdt_first);
    // gdt_pretty_print(&gdt_first);
}
