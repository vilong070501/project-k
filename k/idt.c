#include "include/k/idt.h"
#include "include/k/isr.h"
#include "include/k/pic_8259.h"

struct IDT idt_entries[NB_OF_EXCEPTIONS];
struct IDT_PTR idt_first;

void load_idt(struct IDT *idt_entries)
{
    asm volatile ("lidt %0" : /* No output */ : "m"(*idt_entries) : "memory");
    asm volatile ("ret");
}

void idt_set_entry(u8 intnum, u32 isr, u16 selector, u8 flags)
{
	struct IDT *entry = &idt_entries[intnum];

	entry->base_low = (u32)isr & 0xFFFF;
	entry->segment_selector = selector;
	entry->zero = 0;
	entry->type = flags;
	entry->base_high = ((u32)isr >> 16) & 0xFFFF;
}

void init_idt()
{
	init_pic_8259();
	idt_set_entry(0, (u32)exception_0, 0x08, 0x8E);
    idt_set_entry(1, (u32)exception_1, 0x08, 0x8E);
    idt_set_entry(2, (u32)exception_2, 0x08, 0x8E);
    idt_set_entry(3, (u32)exception_3, 0x08, 0x8E);
    idt_set_entry(4, (u32)exception_4, 0x08, 0x8E);
    idt_set_entry(5, (u32)exception_5, 0x08, 0x8E);
    idt_set_entry(6, (u32)exception_6, 0x08, 0x8E);
    idt_set_entry(7, (u32)exception_7, 0x08, 0x8E);
    idt_set_entry(8, (u32)exception_8, 0x08, 0x8E);
    idt_set_entry(9, (u32)exception_9, 0x08, 0x8E);
    idt_set_entry(10, (u32)exception_10, 0x08, 0x8E);
    idt_set_entry(11, (u32)exception_11, 0x08, 0x8E);
    idt_set_entry(12, (u32)exception_12, 0x08, 0x8E);
    idt_set_entry(13, (u32)exception_13, 0x08, 0x8E);
    idt_set_entry(14, (u32)exception_14, 0x08, 0x8E);
    idt_set_entry(15, (u32)exception_15, 0x08, 0x8E);

	idt_first.limit_size = sizeof(idt_entries) - 1;
	idt_first.base_address = (struct IDT*)&idt_entries;

	load_idt((struct IDT*)&idt_first);
	asm volatile ("sti");
}
