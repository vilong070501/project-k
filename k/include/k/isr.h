#ifndef ISR_H
#define ISR_H

#include "types.h"

typedef struct
{
	// defined in the reverse order they are pushed
	u32 ds;										// data segment pushed by us
	u32 edi, esi, ebp, esp, ebx, edx, ecx, eax; // pushed by pusha
	u32 int_no, err_code; 						// interrupt number and error code
	u32 eip, cs, eflags, user_esp, ss; 			// pushed by the processor automatically
} __attribute__((packed)) Registers;

// IRQ default constants
#define IRQ_BASE            0x20
#define IRQ0_TIMER          0x00
#define IRQ1_KEYBOARD       0x01
#define IRQ2_CASCADE        0x02
#define IRQ3_SERIAL_PORT2   0x03
#define IRQ4_SERIAL_PORT1   0x04
#define IRQ5_RESERVED       0x05
#define IRQ6_DISKETTE_DRIVE 0x06
#define IRQ7_PARALLEL_PORT  0x07
#define IRQ8_CMOS_CLOCK     0x08
#define IRQ9_CGA            0x09
#define IRQ10_RESERVED      0x0A
#define IRQ11_RESERVED      0x0B
#define IRQ12_AUXILIARY     0x0C
#define IRQ13_FPU           0x0D
#define IRQ14_HARD_DISK     0x0E
#define IRQ15_RESERVED      0x0F

// ISR function prototype
typedef void (*ISRHandler)(Registers*);

void ISR_handler(Registers *reg);
void init_ISR(void);
void ISR_RegisterHandler(int int_no, ISRHandler handler);

#endif
