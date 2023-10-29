#include "include/k/idt.h"
#include "include/k/isr.h"
#include "include/k/pic_8259.h"
#include "../libs/libc/include/stdio.h"


// For both exceptions and irq interrupt
ISR g_interrupt_handlers[NB_OF_INTERRUPT_ENTRIES] = {NULL};

// for more details, see Intel manual -> Interrupt & Exception Handling
char *exception_messages[32] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
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
    "Control Protection Exception",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Hypervisor Injection Exception",
    "VMM Communication Exception",
    "Security Exception",
    "Reserved",
    "Reserved"
};

/**
 * register given handler to interrupt handlers at given num
 */
void isr_register_interrupt_handler(int num, ISR handler)
{
    printf("IRQ %d registered\n", num);
    if (num < NB_OF_INTERRUPT_ENTRIES)
    {
        g_interrupt_handlers[num] = handler;
        enable_idt_entry(num);
    }
}

/*
 * turn off current interrupt
*/
void isr_end_interrupt(int num)
{
    send_pic_eoi(num);
}


static void print_registers(registers *reg)
{
    printf("REGISTERS:\n");
    printf("err_code=%d\n", reg->err_code);
    printf("eax=0x%x, ebx=0x%x, ecx=0x%x, edx=0x%x\n", reg->eax, reg->ebx, reg->ecx, reg->edx);
    printf("edi=0x%x, esi=0x%x, ebp=0x%x, esp=0x%x\n", reg->edi, reg->esi, reg->ebp, reg->esp);
    printf("eip=0x%x, cs=0x%x, ss=0x%x, eflags=0x%x, user_esp=0x%x\n", reg->eip, reg->cs, reg->cs, reg->eflags, reg->user_esp);
}

/**
 * invoke isr routine and send eoi to pic,
 * being called in irq.S
 */
void isr_irq_handler(registers *reg)
{
    if (reg->int_no < NB_OF_INTERRUPT_ENTRIES && g_interrupt_handlers[reg->int_no] != NULL)
	{
        ISR handler = g_interrupt_handlers[reg->int_no];
        handler(reg);
    }
    send_pic_eoi(reg->int_no);
}

/**
 * invoke exception routine,
 * being called in exception.S
 */
void isr_exception_handler(registers *reg)
{
    if (reg->int_no < 32)
	{
        printf("isr_exception_handler\n");
        printf("EXCEPTION: %s\n", exception_messages[reg->int_no]);
        print_registers(reg);
        for (;;)
            ;
    }
    if (reg->int_no < NB_OF_INTERRUPT_ENTRIES && g_interrupt_handlers[reg->int_no] != NULL)
	{
        ISR handler = g_interrupt_handlers[reg->int_no];
        handler(reg);
    }
}
