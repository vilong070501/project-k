#ifndef IO_H_
#define IO_H_

#include <k/types.h>

#define UNUSED_PORT 0x80

static inline void outb(u16 port, u8 val)
{
	asm volatile ("outb %0, %1" : /* No output */ : "a"(val), "d"(port));
}

static inline u8 inb(u16 port)
{
	u8 res;

	asm volatile ("inb %1, %0" : "=&a"(res) : "d"(port));

	return res;
}

static inline void outw(u16 port, u16 val)
{
	asm volatile ("outw %0, %1" : /* No output */ : "a"(val), "d"(port));
}

static inline u16 inw(u16 port)
{
	u16 res;

	asm volatile ("inw %1, %0" : "=&a"(res) : "d"(port));

	return res;
}

static inline void outl(u16 port, u16 val)
{
	asm volatile ("outl %%eax, %%dx" : /* No output */ : "d"(port), "a"(val));
}

static inline u16 inl(u16 port)
{
	u16 res;

	asm volatile ("inl %%dx, %%eax" : "=&a"(res) : "d"(port));

	return res;
}

static inline void iowait()
{
	outb(UNUSED_PORT, 0);
}

#endif				/* !IO_H_ */
