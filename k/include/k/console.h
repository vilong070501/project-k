#ifndef CONSOLE_H
#define CONSOLE_H

#include "../../libvga.h"
#include "types.h"

#define MAXIMUM_PAGES   16

#define SCROLL_UP       1
#define SCROLL_DOWN     2

void clear_console(VGA_COLOR_TYPE fore_color, VGA_COLOR_TYPE back_color);

// Initialize console
void init_console(VGA_COLOR_TYPE fore_color, VGA_COLOR_TYPE back_color);
void console_scroll(int type);
void console_putchar(char ch);
void console_putstr(const char *str);
// Revert back the last printed character and set
void console_ungetchar(void);
// Revert back printed characters until n characters
void console_ungetchar_bound(u8 n);
void console_gotoxy(u16 x, u16 y);
void console_printf(const char *format, ...);

#endif