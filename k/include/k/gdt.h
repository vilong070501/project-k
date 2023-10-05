#ifndef GDT_H
#define GDT_H

#include "types.h"

#define NULL_SEGMENT 0
#define KERNEL_CODE_SEGMENT 1
#define KERNEL_DATA_SEGMENT 2
#define USER_CODE_SEGMENT 3
#define USER_DATA_SEGMENT 4
#define TSS_SEGMENT 5

struct GDT
{
    u16 segment_limit;  // segment limit first 0-15 bits
    u16 base_low;       // base first 0-15 bits
    u8 base_middle;     // base 16-23 bits
    u8 access;            // access byte
    u8 granularity;     // high 4 bits (flags) low 4 bits (limit 4 last bits)
    u8 base_high;       // base 24-31 bits
} __attribute__((packed));

/* Define the pointer that we used to load GDT in assembly code */
struct GDT_PTR
{
    u16 limit_size;             //limit size of all GDT segments
    struct GDT* base_address;   // base address of the first GDT segment
} __attribute__((packed));

extern void load_gdt(struct GDT* gdt_entries);
extern void create_descriptor(int segment, u32 base_low, u32 limit,
                        u8 access, u8 granularity);
extern void init_gdt();
#endif
