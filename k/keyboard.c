#include "io.h"
#include "include/k/keyboard.h"
#include "include/k/kstd.h"
#include "include/k/isr.h"

int key = 0;

int getKey(void)
{
    int i, scancode = 0;

    // get scancode until status is on(key pressed)
    for (i = 1000; i > 0; i++) {
        // Check if scan code is ready
        if ((inb(KEYBOARD_STATUS_PORT) & 1) == 0) continue;
        // Get the scan code
        scancode = inb(KEYBOARD_DATA_PORT);
        break;
    }
    if (i > 0)
        return scancode;
    return -1;
}

void keyboard_handler(registers *reg)
{
    key = 0;
    int scancode = getKey();
    if (scancode & 0x80)
    {
        // Key realeased
    }
    else
    {
        //Key pressed
        key = scancode;
    }
}

void keyboard_init()
{
    isr_register_interrupt_handler(IRQ_BASE + 1, keyboard_handler);
}

int getScancode()
{
    int code;
    while (key <= 0); // Loop until a key is pressed
    code = key;
    key = 0;
    return code;
}