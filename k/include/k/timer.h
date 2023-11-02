#ifndef TIMER_H
#define TIMER_H

#include "types.h"

#define TIMER_INPUT_CLOCK_FREQUENCY 1193182
#define TIMER_CHANNEL_0_DATA_PORT   0x40
#define TIMER_CHANNEL_1_DATA_PORT   0x41
#define TIMER_CHANNEL_2_DATA_PORT   0x42
#define TIMER_COMMAND_PORT          0x43

typedef struct {
    u32 timeout;    // in millisecond
    void *user;
} TIMER_FUNC_ARGS;

typedef void (*TIMER_FUNCTION)(TIMER_FUNC_ARGS *);

#define MAXIMUM_TIMER_FUNCTIONS 32

typedef struct {
    u32 current_index;
    // timer functions to be called when the ticks reached an irq handler
    TIMER_FUNCTION functions[MAXIMUM_TIMER_FUNCTIONS];
    // arguments of each above timer functions
    TIMER_FUNC_ARGS func_args[MAXIMUM_TIMER_FUNCTIONS];
} TIMER_FUNCTION_MANAGER;

void init_timer();
void sleep(int sec);
void timer_register_function(TIMER_FUNCTION function, TIMER_FUNC_ARGS *args);
void add_timer_function(TIMER_FUNCTION function, u32 timeout);

#endif