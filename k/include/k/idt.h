#ifndef IDT_H
#define IDT_H

#include "types.h"

#define NB_OF_INTERRUPT_ENTRIES 256

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

typedef enum
{
	IDT_FLAG_GATE_TASK			= 0x5,
	IDT_FLAG_GATE_16BIT_INT		= 0x6,
	IDT_FLAG_GATE_16BIT_TRAP	= 0x7,
	IDT_FLAG_GATE_32BIT_INT		= 0xE,
	IDT_FLAG_GATE_32BIT_TRAP	= 0xF,

	IDT_FLAG_RING0				= (0 << 5),
	IDT_FLAG_RING1				= (1 << 5),
	IDT_FLAG_RING2				= (2 << 5),
	IDT_FLAG_RING3				= (3 << 5),

	IDT_FLAG_PRESENT			= 0x80,
} IDT_FLAGS;


// asm function defined in load_idt.S
extern void load_idt(IDT_PTR *idt_first);

void enable_idt_gate(int intnum);
void disable_idt_gate(int intnum);
// Fill entries of IDT
void idt_set_entry(int intnum, u32 isr, u16 selector, u8 flags);
// Initialize IDT
void init_IDT();

#endif
