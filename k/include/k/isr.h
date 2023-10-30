#ifndef ISR_H
#define ISR_H

#include "types.h"

typedef struct
{
	u32 ds;
	u32 edi, esi, ebp, esp, ebx, edx, ecx, eax; // pushed by pusha
	u32 int_no, err_code; 						// interrupt number and error code
	u32 eip, cs, eflags, user_esp, ss; 			// pushed by the processor automatically
} __attribute__((packed)) Registers;

// ISR function prototype
typedef void (*ISR)(Registers*);

/**
 * register given handler to interrupt handlers at given num
 */
void isr_register_interrupt_handler(int num, ISR handler);

/*
 * turn off current interrupt
*/
void isr_end_interrupt(int num);

/**
 * invoke exception routine,
 * being called in exception.S
 */
void isr_exception_handler(Registers *reg);

/**
 * invoke isr routine and send eoi to pic,
 * being called in irq.S
 */
void isr_irq_handler(Registers *reg);


// defined in exception.S
extern void exception_0();
extern void exception_1();
extern void exception_2();
extern void exception_3();
extern void exception_4();
extern void exception_5();
extern void exception_6();
extern void exception_7();
extern void exception_8();
extern void exception_9();
extern void exception_10();
extern void exception_11();
extern void exception_12();
extern void exception_13();
extern void exception_14();
extern void exception_15();

// defined in irq.S
extern void irq_0();
extern void irq_1();
extern void irq_2();
extern void irq_3();
extern void irq_4();
extern void irq_5();
extern void irq_6();
extern void irq_7();
extern void irq_8();
extern void irq_9();
extern void irq_10();
extern void irq_11();
extern void irq_12();
extern void irq_13();
extern void irq_14();
extern void irq_15();

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



#endif
