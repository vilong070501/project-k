#ifndef GDT_H
#define GDT_H

#include "types.h"

#define NB_OF_GDT_ENTRIES 6

#define NULL_SEGMENT 0
#define KERNEL_CODE_SEGMENT 1
#define KERNEL_DATA_SEGMENT 2
#define USER_CODE_SEGMENT 3
#define USER_DATA_SEGMENT 4
#define TSS_SEGMENT 5

#define KERNEL_CODE_SEGMENT_OFFSET 0x08
#define KERNEL_DATA_SEGMENT_OFFSET 0x10
#define USER_CODE_SEGMENT_OFFSET 0x18
#define USER_DATA_SEGMENT_OFFSET 0x20

typedef struct
{
    u16 segment_limit;  // segment limit first 0-15 bits
    u16 base_low;       // base first 0-15 bits
    u8 base_middle;     // base 16-23 bits
    u8 access;          // access byte
    u8 granularity;     // high 4 bits (flags) low 4 bits (limit 4 last bits)
    u8 base_high;       // base 24-31 bits
} __attribute__((packed)) GDT_ENTRY;

/* Define the pointer that we used to load GDT in assembly code */
typedef struct
{
    u16 limit_size;             //limit size of all GDT segments
    GDT_ENTRY* base_address;   // base address of the first GDT segment
} __attribute__((packed)) GDT_PTR;


// asm gdt functions, defined in load_gdt.S
extern void load_gdt();
// Create an entry in GDT
void gdt_set_entry(int segment, u32 base_low, u32 limit,
                        u8 access, u8 granularity);
// Initialize GDT
void init_GDT();
#endif
