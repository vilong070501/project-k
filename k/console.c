#include "include/k/console.h"

#include "include/k/types.h"
#include "../libs/libc/include/string.h"
#include "libvga.h"

// video buffer array
static u16 *vga_buffer;
// index for video buffer array
static u32 vga_index;
// cursor position
static u8 cursor_pos_x = 0, cursor_pos_y = 0;
// fore and back color values
VGA_COLOR_TYPE fore_color = COLOR_WHITE, back_color = COLOR_BLACK;

static u16 temp_pages[MAXIMUM_PAGES][VGA_TEXT_TOTAL_ITEMS];
u32 current_temp_page = 0;

/**
 * Clear video buffer array
*/
void clear_console(VGA_COLOR_TYPE fore, VGA_COLOR_TYPE back)
{
    u32 i;
    for (i = 0; i < VGA_TEXT_TOTAL_ITEMS; i++)
    {
        vga_buffer[i] = vga_text_item_entry(0, fore, back);
    }
    vga_index = 0;
    cursor_pos_x = 0;
    cursor_pos_y = 0;
    vga_text_set_cursor_pos(cursor_pos_x, cursor_pos_y);
}

/**
 * Initialize console
*/
void init_console(VGA_COLOR_TYPE fore, VGA_COLOR_TYPE back)
{
    vga_buffer = (u16 *)VGA_TEXT_ADDRESS;
    fore_color = fore;
    back_color = back;
    cursor_pos_x = 0;
    cursor_pos_y = 0;
    clear_console(fore_color, back_color);
}

void console_scroll(int type)
{
    u32 i;
    if (type == SCROLL_UP)
    {
        // Scroll up
        if (current_temp_page > 0)
            current_temp_page--;
    }
    else
    {
        // Scroll down
        current_temp_page++;
    }
    current_temp_page %= MAXIMUM_PAGES;
    for (i = 0; i < VGA_TEXT_TOTAL_ITEMS; i++)
    {
        vga_buffer[i] = temp_pages[current_temp_page][i];
    }
}

/**
 * Increase vga_index by width of vga width
*/
static void console_newline(void)
{
    u32 i;

    for (i = 0; i < VGA_TEXT_TOTAL_ITEMS; i++)
    {
        temp_pages[current_temp_page][i] = vga_buffer[i];
    }

    if (cursor_pos_y >= VGA_TEXT_HEIGHT)
    {
        current_temp_page++;
        cursor_pos_x = 0;
        cursor_pos_y = 0;
        clear_console(fore_color, back_color);
    }
    else
    {
        vga_index += VGA_TEXT_WIDTH - (vga_index % VGA_TEXT_WIDTH);
        cursor_pos_x = 0;
        cursor_pos_y++;
        vga_text_set_cursor_pos(cursor_pos_x, cursor_pos_y);
    }
}

/**
 * Assign ASCII character to video buffer
*/
void console_putchar(char ch)
{
    if (ch == ' ')
    {
        vga_buffer[vga_index++] = vga_text_item_entry(' ', fore_color, back_color);
        vga_text_set_cursor_pos(cursor_pos_x, cursor_pos_y);
        cursor_pos_x++;
    }
    else if (ch == '\t')
    {
        for (int i = 0; i < 4; i++)
        {
            vga_buffer[vga_index++] = vga_text_item_entry(' ', fore_color, back_color);
            vga_text_set_cursor_pos(cursor_pos_x, cursor_pos_y);
            cursor_pos_x++;
        }
    }
    else if (ch == '\n')
    {
        console_newline();
    }
    else
    {
        if (ch > 0)
        {
            vga_buffer[vga_index++] = vga_text_item_entry(ch, fore_color, back_color);
            cursor_pos_x++;
            vga_text_set_cursor_pos(cursor_pos_x, cursor_pos_y);
        }
    }
}

void console_putstr(const char *str)
{
    u32 index = 0;
    while (str[index])
    {
        if (str[index] == '\n')
            console_newline();
        else
            console_putchar(str[index]);
        index++;
    }
}

/**
 * Revert back the last printed character and set 0 to it
*/
void console_ungetchar(void)
{
    if (vga_index > 0)
    {
        vga_buffer[vga_index--] = vga_text_item_entry(0, fore_color, back_color);
        if (cursor_pos_x > 0)
        {
            vga_text_set_cursor_pos(cursor_pos_x, cursor_pos_y);
            cursor_pos_x--;
        }
        else
        {
            cursor_pos_x = VGA_TEXT_WIDTH;
            if (cursor_pos_y > 0)
            {
                cursor_pos_y--;
                vga_text_set_cursor_pos(cursor_pos_x, cursor_pos_y);
                cursor_pos_x--;
            }
            else
            {
                cursor_pos_y = 0;
            }
        }
    }

    vga_buffer[vga_index] = vga_text_item_entry(0, fore_color, back_color);
}

/**
 * Revert back printed characters until n characters
*/
void console_ungetchar_bound(u8 n)
{
    if ((vga_index % VGA_TEXT_WIDTH > n) && (n > 0))
    {
        vga_buffer[vga_index--] = vga_text_item_entry(0, fore_color, back_color);
        if (cursor_pos_x >= n)
        {
            vga_text_set_cursor_pos(cursor_pos_x, cursor_pos_y);
            cursor_pos_x--;
        }
        else
        {
            cursor_pos_x = VGA_TEXT_WIDTH;
            if (cursor_pos_y > 0)
            {
                cursor_pos_y--;
                vga_text_set_cursor_pos(cursor_pos_x, cursor_pos_y);
                cursor_pos_x--;
            }
            else
            {
                cursor_pos_y = 0;
            }
        }
    }

    vga_buffer[vga_index] = vga_text_item_entry(0, fore_color, back_color);
}

void console_gotoxy(u16 x, u16 y)
{
    vga_index = (VGA_TEXT_WIDTH * y) + x;
    cursor_pos_x = x;
    cursor_pos_y = y;
    vga_text_set_cursor_pos(cursor_pos_x, cursor_pos_y);
}

void itoa(char *buf, char base, int d)
{
    char *p = buf;
    char *p1, *p2;
    unsigned long ud = d;
    int divisor = 10;

    /* If %d is specified and D is minus, pu '-' in the head */
    if (base == 'd' && d < 0)
    {
        *p++ = '-';
        buf++;
        ud = -d;
    }
    else if (base == 'x')
    {
        divisor = 16;
    }

    /* Divide Ud by divisor until Ud == 0 */
    do {
        int remainder = ud % divisor;
        *p++ = (remainder < 10) ? remainder + '0' : remainder + 'a' - 10;
    } while (ud /= divisor);

    /* Terminate buf */
    *p = '\0';

    /* Reverse buf */
    p1 = buf;
    p2 = p - 1;
    while (p1 < p2)
    {
        char tmp = *p1;
        *p1 = *p2;
        *p2 = tmp;
        p1++;
        p2--;
    }
}

void console_printf(const char *format, ...)
{
    char **arg = (char **)&format;
    int c;
    char buf[32];

    arg++;

    memset(buf, 0, sizeof(buf));

    while((c = *format++) != 0)
    {
        if (c != '%')
        {
            console_putchar(c);
        }
        else
        {
            char *p1, *p2;
            int pad0 = 0, pad = 0;

            c = *format++;
            if (c == '0')
            {
                // Padding
                pad0 = 1;
                c = *format++;
            }

            if (c >= '0' && c <= '9')
            {
                pad = c - '0';
            }

            switch(c)
            {
            case 'd':
            case 'u':
            case 'x':
                itoa(buf, c, *((int *)arg++));
                p1 = buf;
                goto string;
                break;
            case 's':
                p1 = *arg++;
                if (!p1)
                    p1 = "(null)";

            string:
                for (p2 = p1; *p2; p2++)
                    ; /* Empty loop */
                for (; p2 < p1 + pad; p2++)
                    console_putchar(pad0 ? '0' : ' ');
                while (*p1)
                    console_putchar(*p1++);
                break;

            default:
                console_putchar(*((int *)arg++));
                break;
            }
        }
    }
}
