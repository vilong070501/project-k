#include "include/k/idt.h"
#include "include/k/isr.h"
#include "include/k/pic_8259.h"
#include "../libs/libc/include/stdio.h"
#include "isr_init_entries.h"


// For both exceptions and irq interrupt
ISRHandler interrupt_handlers[NB_OF_INTERRUPT_ENTRIES];

extern void kernel_panic();

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
    "Reserved",
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
    "Reserved",
    "Hypervisor Injection Exception",
    "VMM Communication Exception",
    "Security Exception",
    "Reserved"
};


void print_registers(Registers *regs)
{
    printf("eax=%x, ebx=%x, ecx=%x, edx=%x, esi=%x, edi=%x\n",
    regs->eax, regs->ebx, regs->ecx, regs->edx, regs->esi, regs->edi);
    printf("esp=%x, ebp=%x, eip=%x, eflags=%x, cs=%x, ds=%x, ss=%x\n",
    regs->user_esp, regs->ebp, regs->eip, regs->eflags, regs->cs, regs->ds, regs->ss);
}

void ISR_handler(Registers *regs)
{
    if (interrupt_handlers[regs->int_no] != NULL)
    {
        interrupt_handlers[regs->int_no](regs);
    }
    else if (regs->int_no >= 32)
    {
        printf("Unhandled interrupt %s\n", regs->int_no);
    }
    else
    {
        printf("Unhandled exception %d: \"%s\"\n", regs->int_no, exception_messages[regs->int_no]);
        print_registers(regs);
        kernel_panic();
    }
}

void init_ISR(void)
{
    ISR_InitializeEntries();
    for (int i = 0; i < NB_OF_INTERRUPT_ENTRIES; i++)
    {
        enable_idt_gate(i);
    }
    disable_idt_gate(0x80);
}

void ISR_RegisterHandler(int int_no, ISRHandler handler)
{
    interrupt_handlers[int_no] = handler;
    enable_idt_gate(int_no);
}