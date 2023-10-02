#ifndef WRITE_H
#define WRITE_H

#include <stddef.h>

#define PORT 0x3F8 // COM1

int write(const char *buf, size_t count);

#endif
