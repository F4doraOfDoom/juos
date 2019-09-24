#include <stdio.h>

int puts(const char* s)
{
    while(*s)
    {
        putchar(*s);
        s++;
    }

    return 1;
}