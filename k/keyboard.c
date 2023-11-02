#include "io.h"
#include "include/k/keyboard.h"
#include "include/k/kstd.h"
#include "include/k/irq.h"
#include "include/k/isr.h"

#include "../libs/libc/include/ctype.h"

#include "include/k/console.h"

static int caps_lock = 0;
static int shift_pressed = 0;

int key = 0;
char ch = 0;

char scan_code_chars[128] = {
     0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',   
    '\t', /* <-- Tab */
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',     
        0, /* <-- control key */
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',  0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',   0,
    '*',
    0,  /* Alt */
    ' ',  /* Space bar */
    0,  /* Caps lock */
    0,  /* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,  /* < ... F10 */
    0,  /* 69 - Num lock*/
    0,  /* Scroll Lock */
    0,  /* Home key */
    0,  /* Up Arrow */
    0,  /* Page Up */
    '-',
    0,  /* Left Arrow */
    0,
    0,  /* Right Arrow */
    '+',
    0,  /* 79 - End key*/
    0,  /* Down Arrow */
    0,  /* Page Down */
    0,  /* Insert Key */
    0,  /* Delete Key */
    0,   0,   0,
    0,  /* F11 Key */
    0,  /* F12 Key */
    0,  /* All other keys are undefined */
};

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

char alternate_chars(char c)
{
    switch(c)
    {
    case '`': return '~';
    case '1': return '!';
    case '2': return '@';
    case '3': return '#';
    case '4': return '$';
    case '5': return '%';
    case '6': return '^';
    case '7': return '&';
    case '8': return '*';
    case '9': return '(';
    case '0': return ')';
    case '-': return '_';
    case '=': return '+';
    case '[': return '{';
    case ']': return '}';
    case '\\': return '|';
    case ';': return ':';
    case '\'': return '\"';
    case ',': return '<';
    case '.': return '>';
    case '/': return '?';
    default: return c;
    }
}

void keyboard_handler(Registers *reg)
{
    int scancode;
    ch = 0;
    scancode = getKey();
    key = scancode;
    if (scancode & 0x80)
    {
        // Key realeased
    }
    else
    {
        //Key pressed
        switch(scancode)
        {
        case KEY_MAJLOCK:
            if (caps_lock == 0)
                caps_lock = 1;
            else
                caps_lock = 0;
            break;
        case KEY_ENTER:
            ch = '\n';
            break;
        case KEY_TAB:
            ch = '\t';
            break;
        case KEY_LSHIFT:
            shift_pressed = 1;
            break;
        default:
            ch = scan_code_chars[scancode];
            // If caps_lock is on, convert to upper
            if (caps_lock)
            {
                // If shift is pressed before
                if (shift_pressed)
                    // Replace alternate chars
                    ch = alternate_chars(ch);
                else
                    ch = toupper(ch);
            }
            else
            {
                if (shift_pressed)
                {
                    if (isalpha(ch))
                        ch = toupper(ch);
                    else
                        // Replace alternate chars
                        ch = alternate_chars(ch);
                }
                else
                {
                    ch = scan_code_chars[scancode];
                }
                shift_pressed = 0;
            }
            break;
        }
        console_printf("%c", ch);
    }
}

void init_keyboard(void)
{
    IRQ_RegisterHandler(1, keyboard_handler);
}

int keyboard_getScancode(void)
{
    int code;
    while (key <= 0); // Loop until a key is pressed
    code = key;
    ch = 0;
    key = 0;
    return code;
}

/**
 * A blocking character read
*/
char keyboard_getChar(void)
{
    char c;
    while (ch <= 0); // Loop until a key is pressed
    c = ch;
    ch = 0;
    key = 0;
    return c;
}