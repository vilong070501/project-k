/*
 * Copyright (c) LSE
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY LSE AS IS AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL LSE BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <k/kstd.h>

#include "multiboot.h"

#include "../libs/libc/include/stdio.h"
#include "include/k/console.h"
#include "include/k/hal.h"
#include "include/k/irq.h"
#include "include/k/keyboard.h"
#include "include/k/timer.h"

void function_1(TIMER_FUNC_ARGS *args) {
    console_printf("Executing function_1() after %d millisecond\n", args->timeout);
}

void function_2(TIMER_FUNC_ARGS *args) {
    console_printf("Executing function_2() after %d millisecond\n", args->timeout);
}

void k_main(unsigned long magic, multiboot_info_t *info)
{
	(void)magic;
	(void)info;

    init_serial();

    HAL_initialize();

    /******************** Test for Exception handler ********************/
    // Raise Division by zero exception
    // asm volatile ("mov $0, %eax");
    // asm volatile ("div %eax");
    // asm volatile("ret");

    // Raise another Division by zero exception due to Overflow
    // asm volatile ("mov 0xffffffff, %eax");
    // asm volatile ("mov 0xffffffff, %edx");
    // asm volatile ("mov $0x2, %ebx");
    // asm volatile ("div %ebx");
    // asm volatile("ret");

    /******************** Test for Timer module ********************/
    // add_timer_function(function_1, 200);
    // add_timer_function(function_2, 300);

    /******************** Test for Keyboard module ********************/
    // console_printf("Type something...\n");

    // while(1)
    // {
    //     console_printf("%c", keyboard_getChar());
    // }
  
    /******************** Test for VGA graphic module ********************/
    // VGA_graphics_fill_color(COLOR_WHITE);
    // VGA_graphics_draw_rect(10, 10, 10, 10, COLOR_GREEN);

	char star[4] = "|/-\\";
	char *fb = (void *)0xb8000;

    printf("Test for function write\n");
	for (unsigned i = 0; ; ) {
	*fb = star[i++ % 4];
	}

	for (;;)
	asm volatile ("hlt");
}
