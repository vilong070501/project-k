#include "include/k/idt.h"
#include "include/k/isr.h"
#include "include/k/pic_8259.h"
#include "../libs/libc/include/stdio.h"


// For both exceptions and irq interrupt
ISR g_interrupt_handlers[NB_OF_EXCEPTIONS];

// for more details, see Intel manual -> Interrupt & Exception Handling
char *exception_messages[32] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "BOUND Range Exceeded",
    "Invalid Opcode",
    "Device Not Available (No Math Coprocessor)",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack-Segment Fault",
    "General Protection",
    "Page Fault",
    "Unknown Interrupt (intel reserved)",
    "x87 FPU Floating-Point Error (Math Fault)",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

/**
 * register given handler to interrupt handlers at given num
 */
void isr_register_interrupt_handler(int num, ISR handler)
{
    printf("IRQ %d registered\n", num);
    if (num < NB_OF_EXCEPTIONS)
        g_interrupt_handlers[num] = handler;
}

/*
 * turn off current interrupt
*/
void isr_end_interrupt(int num)
{
    send_pic_eoi(num);
}

/**
 * invoke isr routine and send eoi to pic,
 * being called in irq.asm
 */
void isr_irq_handler(struct registers *reg)
{
    if (reg->int_no < NB_OF_EXCEPTIONS &&
		g_interrupt_handlers[reg->int_no] != NULL)
	{
        ISR handler = g_interrupt_handlers[reg->int_no];
        handler(reg);
    }
    send_pic_eoi(reg->int_no);
}

static void print_registers(struct registers *reg)
{
    printf("REGISTERS:\n");
    printf("err_code=%d\n", reg->err_code);
    printf("eax=0x%x, ebx=0x%x, ecx=0x%x, edx=0x%x\n", reg->eax, reg->ebx, reg->ecx, reg->edx);
    printf("edi=0x%x, esi=0x%x, ebp=0x%x, esp=0x%x\n", reg->edi, reg->esi, reg->ebp, reg->esp);
    printf("eip=0x%x, cs=0x%x, ss=0x%x, eflags=0x%x, useresp=0x%x\n", reg->eip, reg->ss, reg->eflags, reg->useresp);
}

/**
 * invoke exception routine,
 * being called in exception.asm
 */
void isr_exception_handler(struct registers reg)
{
    if (reg.int_no < 32)
	{
        printf("EXCEPTION: %s\n", exception_messages[reg.int_no]);
        print_registers(&reg);
        for (;;)
            ;
    }
    if (reg.int_no < 16 && g_interrupt_handlers[reg.int_no] != NULL)
	{
        ISR handler = g_interrupt_handlers[reg.int_no];
        handler(&reg);
    }
}
