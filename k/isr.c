#include "isr.h"
#include "pic_8259.h"
#include "../libs/libc/include/stdio.h"

static struct registers* current_regs = NULL;

void isr_0()
{
	asm volatile ("cli\n");
	asm volatile ("push $0\n");
	asm volatile ("push $0\n");
	asm volatile ("jmp isr\n");
}

void isr_1()
{
	asm volatile ("cli\n");
	asm volatile ("push $1\n");
	asm volatile ("push $1\n");
	asm volatile ("jmp isr\n");
}

void isr_2()
{
	asm volatile ("cli\n");
	asm volatile ("push $2\n");
	asm volatile ("push $2\n");
	asm volatile ("jmp isr\n");
}

void isr_3()
{
	asm volatile ("cli\n");
	asm volatile ("push $3\n");
	asm volatile ("push $3\n");
	asm volatile ("jmp isr\n");
}

void isr_4()
{
	asm volatile ("cli\n");
	asm volatile ("push $4\n");
	asm volatile ("push $4\n");
	asm volatile ("jmp isr\n");
}

void isr_5()
{
	asm volatile ("cli\n");
	asm volatile ("push $5\n");
	asm volatile ("push $5\n");
	asm volatile ("jmp isr\n");
}

void isr_6()
{
	asm volatile ("cli\n");
	asm volatile ("push $6\n");
	asm volatile ("push $6\n");
	asm volatile ("jmp isr\n");
}

void isr_7()
{
	asm volatile ("cli\n");
	asm volatile ("push $7\n");
	asm volatile ("push $7\n");
	asm volatile ("jmp isr\n");
}

void isr_8()
{
	asm volatile ("cli\n");
	asm volatile ("push $8\n");
	asm volatile ("push $8\n");
	asm volatile ("jmp isr\n");
}

void isr_9()
{
	asm volatile ("cli\n");
	asm volatile ("push $9\n");
	asm volatile ("push $9\n");
	asm volatile ("jmp isr\n");
}

void isr_10()
{
	asm volatile ("cli\n");
	asm volatile ("push $10\n");
	asm volatile ("push $10\n");
	asm volatile ("jmp isr\n");
}

void isr_11()
{
	asm volatile ("cli\n");
	asm volatile ("push $11\n");
	asm volatile ("push $11\n");
	asm volatile ("jmp isr\n");
}

void isr_12()
{
	asm volatile ("cli\n");
	asm volatile ("push $12\n");
	asm volatile ("push $12\n");
	asm volatile ("jmp isr\n");
}

void isr_13()
{
	asm volatile ("cli\n");
	asm volatile ("push $13\n");
	asm volatile ("push $13\n");
	asm volatile ("jmp isr\n");
}

void isr_14()
{
	asm volatile ("cli\n");
	asm volatile ("push $14\n");
	asm volatile ("push $14\n");
	asm volatile ("jmp isr\n");
}

void isr_15()
{
	asm volatile ("cli\n");
	asm volatile ("push $15\n");
	asm volatile ("push $15\n");
	asm volatile ("jmp isr\n");
}

/* This function is common for all isr_n.
 * It saves processor state, segments, flags.
 * Some info automatically saved by processor itself
 */
void isr()
{
	// Save all registers
	asm volatile ("pusha\n");
	// Save ds segment
	asm volatile ("mov %ds, %eax\n");
	asm volatile ("push %ax\n");
	// Load new data segment for handling interrupt
	asm volatile ("mov $0x10, %eax\n");
	asm volatile ("mov %eax, %ds\n");
	asm volatile ("mov %eax, %es\n");
	asm volatile ("mov %eax, %fs\n");
	asm volatile ("mov %eax, %gs\n");
	// Call generic interrrupt handler
	asm volatile ("call generic_isr_handler\n");
	// Get original ds segment
	asm volatile ("pop %eax\n");
	// Set its value to other segments
	asm volatile ("mov %eax, %ds\n");
	asm volatile ("mov %eax, %es\n");
	asm volatile ("mov %eax, %fs\n");
	asm volatile ("mov %eax, %gs\n");
	// Restore all registers
	asm volatile ("popa\n");
	// Clean up the pushed error code and pushed ISR number
	asm volatile ("add $8, %esp\n");
	// Store interrupt which was cleard by isr_n()
	asm volatile ("sti\n");
	// Return after handling interrupt
	asm volatile ("iret\n");
}

void generic_isr_handler(struct registers regs)
{
	printf("int_no = %d\n", regs.int_no);
	current_regs = (struct registers*) &regs;
  	switch(regs.int_no){
    	case 0:
      		isr_0_handler();
      		break;
    	case 1:
			isr_1_handler();
			break;
		case 2:
			isr_2_handler();
			break;
		case 3:
			isr_3_handler();
			break;
		case 4:
			isr_4_handler();
			break;
		case 5:
			isr_5_handler();
			break;
		case 6:
			isr_6_handler();
			break;
		case 7:
			isr_7_handler();
			break;
		case 8:
			isr_8_handler();
			break;
		case 9:
			isr_9_handler();
			break;
		case 10:
			isr_10_handler();
			break;
		case 11:
			isr_11_handler();
			break;
		case 12:
			isr_12_handler();
			break;
		case 13:
			isr_13_handler();
			break;
		case 14:
			isr_14_handler();
			break;
		case 15:
			isr_15_handler();
			break;
		default:
			printf("interrupt handler, int_no = %d\n", regs.int_no);
			break;
  	}	
  	current_regs = NULL;
	send_pic_eoi(regs.int_no);
}

void isr_0_handler()
{
	if(current_regs == NULL)
		return;
	printf("isr_0_handler called,  \n");
	printf("Value of eax: %d\n", current_regs->eax);
}

void isr_1_handler()
{
  printf("isr_1_handler called\n");
}

void isr_2_handler()
{
  printf("isr_2_handler called\n");
}

void isr_3_handler()
{
  printf("isr_3_handler called\n");
}

void isr_4_handler()
{
  printf("isr_4_handler called\n");
}

void isr_5_handler()
{
  printf("isr_5_handler called\n");
}

void isr_6_handler()
{
  printf("isr_6_handler called\n");
}

void isr_7_handler()
{
  printf("isr_7_handler called\n");
}

void isr_8_handler()
{
  printf("isr_8_handler called\n");
}

void isr_9_handler()
{
  printf("isr_9_handler called\n");
}

void isr_10_handler()
{
  printf("isr_10_handler called\n");
}

void isr_11_handler()
{
  printf("isr_11_handler called\n");
}

void isr_12_handler()
{
  printf("isr_12_handler called\n");
}

void isr_13_handler()
{
  printf("isr_13_handler called\n");
}

void isr_14_handler()
{
  printf("isr_14_handler called\n");
}

void isr_15_handler()
{
  printf("isr_15_handler called\n");
}
