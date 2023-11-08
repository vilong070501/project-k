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
#ifndef LIBVGA_H
#define LIBVGA_H

#include <stddef.h>
#include "k/types.h"

/* Attribute Controller Registers */
#define VGA_AC_INDEX		0x3C0
#define VGA_AC_WRITE		0x3C0
#define VGA_AC_READ		    0x3C1

/* Miscellaneous Output */
#define VGA_MISC_WRITE		0x3C2
#define VGA_MISC_READ		0x3CC

/* Sequencer Registers */
#define VGA_SEQ_INDEX		0x3C4
#define VGA_SEQ_DATA		0x3C5

/* VGA Color Palette Registers */
#define VGA_DAC_MASK		0x3C6
#define VGA_DAC_READ_INDEX	0x3C7
#define VGA_DAC_WRITE_INDEX	0x3C8
#define VGA_DAC_DATA		0x3C9

/* Graphics Controller Registers */
#define VGA_GC_INDEX 		0x3CE
#define VGA_GC_DATA 		0x3CF

/* CTR Controller Registers */
#define VGA_CRTC_INDEX		0x3D4
#define VGA_CRTC_DATA		0x3D5

/* General Control and Status Registers */
#define VGA_INSTAT_READ	    0x3DA

#define VGA_NUM_SEQ_REGS	5
#define VGA_NUM_CRTC_REGS	25
#define VGA_NUM_GC_REGS	    9
#define VGA_NUM_AC_REGS	    21

#define VGA_GRAPHIC_ADDRESS         0xA0000
#define VGA_GRAPHIC_WIDTH      320
#define VGA_GRAPHIC_HEIGHT     200
#define VGA_GRAPHIC_TOTAL_ITEMS     (VGA_GRAPHIC_WIDTH * VGA_GRAPHIC_HEIGHT)


#define VGA_TEXT_ADDRESS         0xB8000
#define VGA_TEXT_WIDTH      80
#define VGA_TEXT_HEIGHT     25
#define VGA_TEXT_TOTAL_ITEMS     2000

typedef enum {
    COLOR_BLACK,
    COLOR_BLUE,
    COLOR_GREEN,
    COLOR_CYAN,
    COLOR_RED,
    COLOR_MAGENTA,
    COLOR_BROWN,
    COLOR_GREY,
    COLOR_DARK_GREY,
    COLOR_BRIGHT_BLUE,
    COLOR_BRIGHT_GREEN,
    COLOR_BRIGHT_CYAN,
    COLOR_BRIGHT_RED,
    COLOR_BRIGHT_MAGENTA,
    COLOR_YELLOW,
    COLOR_WHITE,
} VGA_COLOR_TYPE;

void libvga_set_palette(unsigned int *new_palette, size_t size);

char *libvga_get_framebuffer(void);

void libvga_switch_mode13h(void);

void libvga_switch_mode3h(void);

/**
 * 16 bits video buffer elements (register ax)
 * 
 * 8 bits(ah) higher:
 * lower 4 bits - fore color
 * higher 4 bits - back color
 * 
 * 8 bits(al) lower:
 * 8 bits: ASCII characters to print
 * 
 * return complete item with fore and back color to be placed at VGA address
*/
u32 vga_text_item_entry(u8 ch, VGA_COLOR_TYPE fore_color, VGA_COLOR_TYPE back_color);

/**
 * Set cursor position to given (x,y)
 * by writing to CRT controller registers
*/
void vga_text_set_cursor_pos(u8 x, u8 y);

/**
 * Disable blinking top-left cursor
 * by writing to CRT controller registers
*/
void vga_disable_cursor(void);

void init_VGA_graphics(void);
void exit_VGA_graphics(void);
void VGA_graphics_clear_color(u8 color);
void VGA_graphics_put_pixel(u16 x, u16 y, u8 color);
void VGA_graphics_draw_line(u16 x1, u16 y1, u16 x2, u16 y2, u8 color);
void VGA_graphics_draw_rect(u16 x, u16 y, u16 width, u16 height, u8 color);
void VGA_graphics_fill_rect(u16 x, u16 y, u16 width, u16 height, u8 color);
void draw_bresenham_circle(int xc, int yc, int x, int y, u8 color);
void VGA_graphics_draw_circle(u16 x, u16 y, u16 radius, u8 color);

#endif				/* !LIBVGA_H */

