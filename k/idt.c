#include "include/k/idt.h"
#include "include/k/isr.h"
#include "include/k/pic_8259.h"

IDT_ENTRY idt_entries[NB_OF_INTERRUPT_ENTRIES];
IDT_PTR idt_first = { sizeof(idt_entries) - 1, idt_entries };

void idt_set_entry(int intnum, u32 isr, u16 selector, u8 flags)
{
	IDT_ENTRY *entry = &(idt_entries[intnum]);

	entry->base_low = isr & 0xFFFF;
	entry->segment_selector = selector;
	entry->zero = 0;
	entry->type = flags;
	entry->base_high = (isr >> 16) & 0xFFFF;
}

void enable_idt_gate(int intnum)
{
    idt_entries[intnum].type |= IDT_FLAG_PRESENT;
}

void disable_idt_gate(int intnum)
{
    idt_entries[intnum].type &= ~(IDT_FLAG_PRESENT);
}

void init_idt()
{
	// init_pic_8259();


	load_idt(&idt_first);
}
