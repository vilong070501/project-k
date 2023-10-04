#include "idt.h"
#include "isr.h"
#include "load_idt.h"

struct IDT idt_entries[16];
struct IDT_PTR idt_first;
void *isr_stub_table[16];

void init_isr_stub_table()
{
	isr_stub_table[0] = isr_0;
	isr_stub_table[1] = isr_1;
	isr_stub_table[2] = isr_2;
	isr_stub_table[3] = isr_3;
	isr_stub_table[4] = isr_4;
	isr_stub_table[5] = isr_5;
	isr_stub_table[6] = isr_6;
	isr_stub_table[7] = isr_7;
	isr_stub_table[8] = isr_8;
	isr_stub_table[9] = isr_9;
	isr_stub_table[10] = isr_10;
	isr_stub_table[11] = isr_11;
	isr_stub_table[12] = isr_12;
	isr_stub_table[13] = isr_13;
	isr_stub_table[14] = isr_14;
	isr_stub_table[15] = isr_15;
}

void set_idt_descriptor(u8 intnum, void* isr, u16 selector, u8 flags)
{
	idt_entries[intnum].base_low = (u32)isr & 0xFFFF;
	idt_entries[intnum].segment_selector = selector;
	idt_entries[intnum].zero = 0;
	idt_entries[intnum].type = flags;
	idt_entries[intnum].base_high = ((u32)isr >> 16) & 0xFFFF;
}

void init_idt()
{
	init_isr_stub_table();
	for (u8 intnum = 0; intnum < 16; intnum++)
	{
		set_idt_descriptor(intnum, isr_stub_table[intnum], 0x08, 0x8E);
	}

	idt_first.limit_size = sizeof(idt_entries) - 1;
	idt_first.base_address = (struct IDT*)&idt_entries;

	load_idt((struct IDT*)&idt_first);
}
