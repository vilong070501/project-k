#ifndef MOUSE_H
#define MOUSE_H

#include "types.h"

// https://wiki.osdev.org/%228042%22_PS/2_Controller

#define MOUSE_DATA_PORT      0x60
#define MOUSE_STATUS_PORT    0x64
#define MOUSE_COMMAND_PORT   0x64

/* Mouse commands */
#define MOUSE_RESET                       0xFF
#define MOUSE_RESEND                      0xFE
#define MOUSE_SET_DEFAULTS                0xF6
#define MOUSE_DISABLE_PACKET_STREAMING    0xF5
#define MOUSE_ENABLE_PACKET_STREAMING     0xF4
#define MOUSE_SAMPLE_RATE                 0xF3
#define MOUSE_IDENITFY                    0xF2
#define MOUSE_REQUEST_SINGLE_PACKET       0xEB
#define MOUSE_STATUS_COMMAND              0xE9
#define MOUSE_RESOLUTION                  0xE8
#define MOUSE_ACKNOWLEDGE                 0xFA

typedef struct {
    u8 left_click: 1;
    u8 right_click: 1;
    u8 middle_click: 1;
    u8 always_1: 1;
    u8 x_sign: 1;
    u8 y_sign: 1;
    u8 x_overflow: 1;
    u8 y_overflow: 1;
} MOUSE_STATUS;

void init_mouse(void);

int mouse_getx(void);
int mouse_gety(void);

#endif