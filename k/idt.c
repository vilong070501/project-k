#include "idt.h"
#include "load_idt.h"

struct IDT idt_entries[16];
struct IDT_PTR idt_first;

void set_idt_descriptor(u8 intnum, u32 isr, u8 flags)
{
	idt_entries[intnum].base_low = isr & 0xFFFF;
	idt_entries[intnum].segment_selector = 0x08;
	idt_entries[intnum].zero = 0;
	idt_entries[intnum].type = flags;
	idt_entries[intnum].base_high = (isr >> 16) & 0xFFFF;
}

void init_idt()
{
	for (u8 intnum = 0; intnum < 16; intnum++)
	{
		set_idt_descriptor(intnum, isr_stub_table[intnum], 0x8E);
	}

	idt_first.limit_size = sizeof(idt_entries) - 1;
	idt_first.base_address = (struct IDT*)&idt_entries;

	load_idt((struct IDT*)&idt_first);
}
