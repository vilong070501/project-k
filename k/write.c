#include <stddef.h>

#include "io.h"
#include "write.h"

int write(const char *buf, size_t count)
{
    for (size_t i = 0; i < count; i++)
    {
        outb(PORT, buf[i]);
    }
    return count;
}
