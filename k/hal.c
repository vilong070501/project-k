#include "include/k/console.h"
#include "include/k/gdt.h"
#include "include/k/hal.h"
#include "include/k/ide.h"
#include "include/k/idt.h"
#include "include/k/irq.h"
#include "include/k/isr.h"
#include "include/k/keyboard.h"
#include "include/k/mouse.h"
#include "include/k/timer.h"
#include "../libs/libc/include/stdio.h"
#include "libvga.h"

void HAL_initialize(void)
{
    /* When come to test VGA graphics */
    // init_console(COLOR_WHITE, COLOR_BLACK);
    init_console(COLOR_WHITE_TEXT, COLOR_BLACK);
    console_printf("Console initialization finished\n");
    init_GDT();
    console_printf("GDT initialization finished\n");
    console_printf("TSS initialization finished\n");
    init_IDT();
    console_printf("IDT initialization finished\n");
    // init_ISR();
    // console_printf("ISR initialization finished\n");
    // init_IRQ();
    // console_printf("IRQ initialization finished\n");
    // init_timer();
    // console_printf("Timer initialization finished\n");
    // vga_disable_cursor();
    // init_keyboard();
    // console_printf("Keyboard initialization finished\n");
    // init_mouse();
    // console_printf("Mouse initialization finished\n");
    // init_ATA();
    // console_printf("ATA initialization finished\n");
    // init_VGA_graphics();
    // console_printf("VGA graphic initialization finished\n");
}