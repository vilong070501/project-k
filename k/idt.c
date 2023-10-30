#include "include/k/idt.h"
#include "include/k/isr.h"
#include "include/k/pic_8259.h"

IDT idt_entries[NB_OF_INTERRUPT_ENTRIES];
IDT_PTR idt_first = { sizeof(idt_entries) - 1, idt_entries };

void idt_set_entry(u8 intnum, u32 isr, u16 selector, u8 flags)
{
	IDT *entry = &(idt_entries[intnum]);

	entry->base_low = isr & 0xFFFF;
	entry->segment_selector = selector;
	entry->zero = 0;
	entry->type = flags | 0x60;
	entry->base_high = (isr >> 16) & 0xFFFF;
}

void enable_idt_gate(u8 intnum)
{
    idt_entries[intnum].type |= 0x80;
}

void disable_idt_gate(u8 intnum)
{
    idt_entries[intnum].type &= ~0x80;
}

void init_idt()
{
	idt_first.limit_size = sizeof(idt_entries) - 1;
	idt_first.base_address = (IDT*)&idt_entries;
	init_pic_8259();
    
	// idt_set_entry(0, (u32)exception_0, 0x08, 0x8E);
    // idt_set_entry(1, (u32)exception_1, 0x08, 0x8E);
    // idt_set_entry(2, (u32)exception_2, 0x08, 0x8E);
    // idt_set_entry(3, (u32)exception_3, 0x08, 0x8E);
    // idt_set_entry(4, (u32)exception_4, 0x08, 0x8E);
    // idt_set_entry(5, (u32)exception_5, 0x08, 0x8E);
    // idt_set_entry(6, (u32)exception_6, 0x08, 0x8E);
    // idt_set_entry(7, (u32)exception_7, 0x08, 0x8E);
    // idt_set_entry(8, (u32)exception_8, 0x08, 0x8E);
    // idt_set_entry(9, (u32)exception_9, 0x08, 0x8E);
    // idt_set_entry(10, (u32)exception_10, 0x08, 0x8E);
    // idt_set_entry(11, (u32)exception_11, 0x08, 0x8E);
    // idt_set_entry(12, (u32)exception_12, 0x08, 0x8E);
    // idt_set_entry(13, (u32)exception_13, 0x08, 0x8E);
    // idt_set_entry(14, (u32)exception_14, 0x08, 0x8E);
    // idt_set_entry(15, (u32)exception_15, 0x08, 0x8E);
    // idt_set_entry(32, (u32)irq_0, 0x08, 0x8E);
    // idt_set_entry(33, (u32)irq_1, 0x08, 0x8E);
    // idt_set_entry(34, (u32)irq_2, 0x08, 0x8E);
    // idt_set_entry(35, (u32)irq_3, 0x08, 0x8E);
    // idt_set_entry(36, (u32)irq_4, 0x08, 0x8E);
    // idt_set_entry(37, (u32)irq_5, 0x08, 0x8E);
    // idt_set_entry(38, (u32)irq_6, 0x08, 0x8E);
    // idt_set_entry(39, (u32)irq_7, 0x08, 0x8E);
    // idt_set_entry(40, (u32)irq_8, 0x08, 0x8E);
    // idt_set_entry(41, (u32)irq_9, 0x08, 0x8E);
    // idt_set_entry(42, (u32)irq_10, 0x08, 0x8E);
    // idt_set_entry(43, (u32)irq_11, 0x08, 0x8E);
    // idt_set_entry(44, (u32)irq_12, 0x08, 0x8E);
    // idt_set_entry(45, (u32)irq_13, 0x08, 0x8E);
    // idt_set_entry(46, (u32)irq_14, 0x08, 0x8E);
    // idt_set_entry(47, (u32)irq_15, 0x08, 0x8E);

	// load_idt(&idt_first);
    asm volatile ("sti");
}
