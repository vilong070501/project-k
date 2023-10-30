#include "include/k/hal.h"
#include "include/k/gdt.h"
#include "include/k/idt.h"

void HAL_initialize(void)
{
    init_gdt();
    init_idt();
}