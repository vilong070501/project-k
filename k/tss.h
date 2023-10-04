#ifndef TSS_H
#define TSS_H

#include "include/k/types.h"

struct TSS
{
	u16 previous_task, __previous_task_reserved;
	u32 esp0;
	u16 ss0, __ss0_reserved;
	u32 esp1;
	u16 ss1, __ss1_reserved;
	u32 esp2;
	u16 ss2, __ss2_reserved;
	u32 cr3;
	u32 eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
	u16 es, __es_reserved;
	u16 cs, __cs_reserved;
	u16 ss, __ss_reserved;
	u16 ds, __ds_reserved;
	u16 fs, __fs_reserved;
	u16 gs, __gs_reserved;
	u16 ldt_selector, __ldt_selector_reserved;
	u16 debug_flag, io_map;
} __attribute__((packed));

extern void load_tss();
extern void init_tss();
void set_tss_stack(u32 esp0);
void print_tss();

#endif