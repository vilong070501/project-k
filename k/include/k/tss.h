#ifndef TSS_H
#define TSS_H

#include "types.h"

struct TSS
{
	u32 previous_task;
	u32 esp0;
	u32 ss0;
	u32 esp1;
	u32 ss1;
	u32 esp2;
	u32 ss2;
	u32 cr3;
	u32 eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
	u32 es;
	u32 cs;
	u32 ss;
	u32 ds;
	u32 fs;
	u32 gs;
	u32 ldt_selector;
	u32 trap;
	u32 io_map;
} __attribute__((packed));

typedef struct TSS TSS;

extern void load_tss();
extern u32 get_eip();
extern void init_TSS();
void set_tss_stack(u32 esp0);
void print_tss();

#endif