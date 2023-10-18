#ifndef ISR_H
#define ISR_H

#include "types.h"

struct registers
{
	u32 ds;
	u32 edi, esi, ebp, esp, ebx, edx, ecx, eax; // pushed by pusha
	u32 int_no, err_code; // interrupt number and error code
	u32 eip, cs, eflags, useresp, ss; // pushed by the processor automatically
};

// ISR function prototype
typedef void (*ISR)(struct registers*);

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
 * being called in exception.asm
 */
void isr_exception_handler(struct registers reg);

/**
 * invoke isr routine and send eoi to pic,
 * being called in irq.asm
 */
void isr_irq_handler(struct registers *reg);


// defined in exception.asm
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


#endif
