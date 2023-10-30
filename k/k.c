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
#include "include/k/hal.h"

// void function_1(TIMER_FUNC_ARGS *args) {
// printf("Executing function_1() after %d millisecond\n", args->timeout);
// }

// void function_2(TIMER_FUNC_ARGS *args) {
// printf("Executing function_2() after %d millisecond\n", args->timeout);
// }

// void add_timer_function(TIMER_FUNCTION function, u32 timeout) {
// TIMER_FUNC_ARGS args = {0};
// args.timeout = timeout;
// timer_register_function(function, &args);
// }


void k_main(unsigned long magic, multiboot_info_t *info)
{
	(void)magic;
	(void)info;

    HAL_initialize();

    // init_console(COLOR_WHITE, COLOR_BLACK);
    // console_printf("Test for print on console - %s with a number %d, and an address %x\n", "My test", 75, VGA_ADDRESS);
    // printf("Test for print on console\n");

    // init_gdt();
    // printf("This line goes after init_gdt() function\r\n");

    // init_tss();
    // printf("This line goes after init_tss() function\r\n");

    // init_idt();
    // printf("This line goes after init_idt() function\r\n");

    // init_timer();
    // printf("This line goes after init_timer() function\r\n");

    // raise division by zero
    // asm volatile("\txorl %edx, %edx");
    // asm volatile("\tmovl $0x7b, %eax");
    // asm volatile("\tmovl $0, %ecx");
    // asm volatile("\tidivl %ecx");

    // keyboard_init();

    // printf("Type something...\n");
    // while(1)
    // {
    //     printf("Key pressed: %d\n", getScancode());
    // }

    // add_timer_function(function_1, 200);
    // add_timer_function(function_2, 2300);
    // while(1) {
    //     printf("Hello\n");
    //     sleep(1);
    // }

	char star[4] = "|/-\\";
	char *fb = (void *)0xb8000;

    printf("Test for function write\n");
	for (unsigned i = 0; ; ) {
	*fb = star[i++ % 4];
	}


	for (;;)
	asm volatile ("hlt");
}
