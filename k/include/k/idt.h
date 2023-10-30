#ifndef IDT_H
#define IDT_H

#include "types.h"

#define NB_OF_INTERRUPT_ENTRIES 256
#define IDT_FLAG_PRESENT 0x80

typedef struct
{
	u16 base_low; 			// lower 16 bits 0-15 of the address to jump to when this interrupt fires
	u16 segment_selector;   // code segment selector in GDT
	u8 zero; 				// unusedn always be zero
	u8 type; 				// types trap, interrupt gates
	u16 base_high;			// upper 16 bits 16-31 of the address to jump to
} __attribute__((packed)) IDT_ENTRY;

typedef struct
{
	u16 limit_size; 			// limit size of all IDT segments
	IDT_ENTRY *base_address; 	// base address of the first IDT segment
} __attribute__((packed)) IDT_PTR;


// asm function defined in load_idt.S
extern void load_idt(IDT_PTR *idt_first);

void enable_idt_gate(u8 intnum);
void disable_idt_gate(u8 intnum);
// Fill entries of IDT
void idt_set_entry(u8 intnum, u32 isr, u16 selector, u8 flags);
// Initialize IDT
void init_idt();

#endif
