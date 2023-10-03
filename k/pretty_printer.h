#ifndef PRETTY_PRINTER_H
#define PRETTY_PRINTER_H

#include "gdt.h"
#include "../libs/libc/include/stdio.h"

void gdt_pretty_print(struct GDT_PTR* gdtr)
{
	size_t nb_entries = (gdtr->limit_size + 1) / sizeof(struct GDT);
	for (size_t i = 0; i < nb_entries; i++)
	{
		struct GDT *entry = (struct GDT*)(gdtr->base_address + i * sizeof(struct GDT));
		printf("GDT[%d]: \r\n", i);
		printf("segment_limit = %d\r\n", entry->segment_limit);
    		printf("base_low      = %d\r\n", entry->base_low);
    		printf("base_middle   = %d\r\n", entry->base_middle);
    		printf("access	      = %d\r\n", entry->access);
    		printf("granularity   = %d\r\n", entry->granularity);
		printf("base_high     = %d\r\n", entry->base_high);
	}
}

#endif
