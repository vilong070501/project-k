#include "include/k/timer.h"

#include "include/k/idt.h"
#include "include/k/isr.h"
#include "include/k/irq.h"
#include "include/k/types.h"
#include "include/k/console.h"
#include "../libs/libc/include/stdio.h"
#include "../libs/libc/include/string.h"
#include "io.h"

// number of ticks since system booted
u32 ticks = 0;
// frequency in hertz
u16 freq_hz = 0;
// timer functions to be called when that ticks reached in irq handler
TIMER_FUNCTION_MANAGER timer_function_manager;

extern void enable_interrupts(void);
extern void disable_interrupts(void);

u32 gettick()
{
    return ticks;
}

void timer_set_frequency(u16 freq)
{
    // Disable interrupts
    disable_interrupts();

    freq_hz = freq;
    u16 divisor = TIMER_INPUT_CLOCK_FREQUENCY / freq;
    // Set mode 2 - Rate Generator
    outb(TIMER_COMMAND_PORT, 0b00110100);
    // Set low byte
    outb(TIMER_CHANNEL_0_DATA_PORT, divisor & 0xFF);
    // Set high byte
    outb(TIMER_CHANNEL_0_DATA_PORT, (divisor >> 8) & 0xFF);

    // Enable interrupts
    enable_interrupts();
}

void timer_handler(Registers *reg)
{
    u32 i;
    TIMER_FUNC_ARGS *args = NULL;
    ticks++;
    for (i = 0; i < MAXIMUM_TIMER_FUNCTIONS; i++)
    {
        args = &(timer_function_manager.func_args[i]);
        if (args->timeout == 0)
            continue;
        if ((ticks % args->timeout) == 0)
            timer_function_manager.functions[i](args);
    }
}

void timer_register_function(TIMER_FUNCTION function, TIMER_FUNC_ARGS *args)
{
    u32 index = 0;
    if (function == NULL || args == NULL)
    {
        console_printf("Error: failed to register timer function %x\n", function);
        return;
    }
    index = (++timer_function_manager.current_index) % MAXIMUM_TIMER_FUNCTIONS;
    timer_function_manager.current_index = index;
    timer_function_manager.functions[index] = function;
    memcpy(&timer_function_manager.func_args[index], args, sizeof(TIMER_FUNC_ARGS));
}

void add_timer_function(TIMER_FUNCTION function, u32 timeout) {
    TIMER_FUNC_ARGS args = {0};
    args.timeout = timeout;
    timer_register_function(function, &args);
}

void init_timer()
{
    // IRQ0 will fire 100 times per second
    timer_set_frequency(100);
    IRQ_RegisterHandler(0, timer_handler);
}

void sleep(int sec)
{
    u32 end = ticks + sec * freq_hz;
    while (ticks < end);
}