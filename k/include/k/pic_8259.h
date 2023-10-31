#ifndef PIC_8259_H
#define PIC_8259_H

#include "types.h"

#define PIC1_COMMAND_PORT    0x20        /* The master PIC's port A */
#define PIC1_DATA_PORT       0x21        /* The master PIC's port B */
#define PIC2_COMMAND_PORT    0xA0        /* The slave PIC's port A */
#define PIC2_DATA_PORT       0xA1        /* The slave PIC's port B */

typedef enum
{
    ICW1_ICW4	     =   0x01,		/* Indicates that ICW4 will be present */
    ICW1_SINGLE	     =   0x02,		/* Single (cascade) mode */
    ICW1_INTERVAL4	 =   0x04,		/* Call address interval 4 (8) */
    ICW1_LEVEL	     =   0x08,      /* Level triggered (edge) mode */
    ICW1_INIT	     =   0x10,		/* Initialization - required! */
} PIC_ICW1;

typedef enum
{
    ICW4_8086	     =   0x01,		/* 8086/88 (MCS-80/85) mode */
    ICW4_AUTO	     =   0x02,		/* Auto (normal) EOI */
    ICW4_BUF_SLAVE	 =   0x08,		/* Buffered mode/slave */
    ICW4_BUF_MASTER	 =   0x0C,		/* Buffered mode/master */
    ICW4_SFNM	     =   0x10,		/* Special fully nested (not) */
} PIC_ICW4;

typedef enum
{
    PIC_EOI          =   0x20,      /* End of interrupt command */
    PIC_READ_IRR     =   0x0A,
    PIC_READ_ISR     =   0x0B,
} PIC_CMD;

void init_PIC_8259(u8 offsetPIC1, u8 offsetPIC2);
void IRQ_set_mask(int irq);
void IRQ_clear_mask(int irq);
void disable_PIC(void);
void send_PIC_EOI(int irq);
u16 readIRQRequestRegister(void);
u16 readInServiceRegister(void);

#endif