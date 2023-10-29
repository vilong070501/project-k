#ifndef PRETTY_PRINTER_H
#define PRETTY_PRINTER_H

#include "include/k/gdt.h"
#include "../libs/libc/include/stdio.h"

void gdt_pretty_print(GDT_PTR* gdtr)
{
	size_t nb_entries = (gdtr->limit_size + 1) / sizeof(GDT);
	for (size_t i = 0; i < nb_entries; i++)
	{
		GDT *entry = (GDT*)(gdtr->base_address + i * sizeof(GDT));
		printf("GDT[%d]: \r\n", i);
		printf("segment_limit = %x\r\n", entry->segment_limit);
		printf("base_low      = %x\r\n", entry->base_low);
		printf("base_middle   = %x\r\n", entry->base_middle);
		printf("access	      = %x\r\n", entry->access);
		printf("granularity   = %x\r\n", entry->granularity);
		printf("base_high     = %x\r\n", entry->base_high);
	}
}

#endif
