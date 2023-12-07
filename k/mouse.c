#include "include/k/mouse.h"

#include "include/k/console.h"
#include "include/k/idt.h"
#include "include/k/irq.h"
#include "include/k/isr.h"
#include "include/k/pic_8259.h"
#include "include/k/types.h"

#include "../libs/libc/include/string.h"

#include "io.h"
#include "libvga.h"

int mouse_x_pos = 0, mouse_y_pos = 0;
MOUSE_STATUS mouse_status;

int mouse_getx(void)
{
    return mouse_x_pos;
}

int mouse_gety(void)
{
    return mouse_y_pos;
}

static void mouse_wait(int type)
{
    u32 timeout = 100000;
    if (type == 0)
    {
        // Suspend until status is 1
        while (timeout--)
        {
            if ((inb(MOUSE_COMMAND_PORT) & 1) == 1)
                return;
        }
    }
    else
    {
        while (timeout--)
        {
            if ((inb(MOUSE_COMMAND_PORT) & 2) == 0)
                return;
        }
    }
}

static void mouse_write(u8 data)
{
    // Sending write command
    mouse_wait(1);
    outb(MOUSE_COMMAND_PORT, 0xD4);
    mouse_wait(1);
    // Finally write data to port
    outb(MOUSE_DATA_PORT, data);
}

static u8 mouse_read()
{
    mouse_wait(0);
    return inb(MOUSE_DATA_PORT);
}

static void mouse_get_status(char status_byte, MOUSE_STATUS *status)
{
    memset(status, 0, sizeof(MOUSE_STATUS));
    if (status_byte & 0x01)
        status->left_click = 1;
    if (status_byte & 0x02)
        status->right_click = 1;
    if (status_byte & 0x04)
        status->middle_click = 1;
    if (status_byte & 0x08)
        status->always_1 = 1;
    if (status_byte & 0x10)
        status->x_sign = 1;
    if (status_byte & 020)
        status->y_sign = 1;
    if (status_byte & 0x40)
        status->x_overflow = 1;
    if (status_byte & 0x80)
        status->y_overflow = 1;
}

static void print_mouse_info(void)
{
    console_gotoxy(0,0);
    console_printf("Mouse Demo X: %d, Y: %d\n", mouse_x_pos, mouse_y_pos);
    if (mouse_status.left_click)
        console_printf("Left button clicked\n");
    if (mouse_status.right_click)
        console_printf("Right button clicked\n");
    if (mouse_status.middle_click)
        console_printf("Middle button clicked\n");
}

void mouse_handler(Registers *reg)
{
    static u8 mouse_cycle = 0;
    static char mouse_byte[3];

    switch (mouse_cycle)
    {
    case 0:
        mouse_byte[0] = mouse_read();
        mouse_get_status(mouse_byte[0], &mouse_status);
        mouse_cycle++;
        break;
    case 1:
        mouse_byte[1] = mouse_read();
        mouse_cycle++;
        break;
    case 2:
        mouse_byte[2] = mouse_read();
        mouse_x_pos += mouse_byte[1];
        mouse_y_pos -= mouse_byte[2];

        if (mouse_x_pos < 0)
            mouse_x_pos	= 0;
        if (mouse_y_pos < 0)
            mouse_y_pos	= 0;
        if (mouse_x_pos > VGA_WIDTH)
            mouse_x_pos	= VGA_WIDTH - 1;
        if (mouse_y_pos > VGA_HEIGHT)
            mouse_y_pos	= VGA_HEIGHT - 1;

        clear_console(COLOR_WHITE, COLOR_BLACK);
        console_gotoxy(mouse_x_pos, mouse_y_pos);
        console_putchar('X');
        print_mouse_info();
        mouse_cycle = 0;
        break;
    }
    send_PIC_EOI(12);
}

/**
 * Available rates: 10, 20, 40, 60, 80, 100, 200
*/
static void set_mouse_rate(u8 rate)
{
    u8 status;
    outb(MOUSE_DATA_PORT, MOUSE_SAMPLE_RATE);
    status = mouse_read();
    if (status != MOUSE_ACKNOWLEDGE)
    {
        console_printf("Error: Failed to send mouse sample rate command\n");
        return;
    }
    outb(MOUSE_DATA_PORT, rate);
    if (status != MOUSE_ACKNOWLEDGE)
    {
        console_printf("Error: Failed to send mouse sample rate data\n");
        return;
    }
}

void init_mouse(void)
{
    u8 status;

    mouse_x_pos = 5;
    mouse_y_pos = 2;

    // Enable mouse device
    mouse_wait(1);
    outb(MOUSE_COMMAND_PORT, 0xA8);

    // Print mouse id
    outb(MOUSE_DATA_PORT, MOUSE_IDENITFY);
    status = mouse_read();
    console_printf("Mouse id = 0x%x\n", status);

    set_mouse_rate(10);

    // Enable the interrupt IRQ12
    mouse_wait(1);
    outb(MOUSE_COMMAND_PORT, 0x20);
    mouse_wait(0);

    // Get and set second bit
    status = inb(MOUSE_DATA_PORT) | 2;

    // Write status to port
    mouse_wait(1);
    outb(MOUSE_COMMAND_PORT, 0x60);
    mouse_wait(1);
    outb(MOUSE_DATA_PORT, status);

    // Set mouse to use default settings
    mouse_write(MOUSE_SET_DEFAULTS);
    status = mouse_read();
    if (status != MOUSE_ACKNOWLEDGE)
    {
        console_printf("Error: Failed to set default settings for mouse\n");
        return;
    }

    // Enable packet streaming to receive
    mouse_write(MOUSE_ENABLE_PACKET_STREAMING);
    status = mouse_read();
    if (status != MOUSE_ACKNOWLEDGE)
    {
        console_printf("Error: Failed to enable packet streaming for mouse\n");
        return;
    }

    IRQ_RegisterHandler(12, mouse_handler);
}
