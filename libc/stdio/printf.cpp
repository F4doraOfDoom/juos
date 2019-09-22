#include <stdio.h>
#include <stdarg.h>

static int print(const unsigned char c)
{
    return 0;
} 

int printf(const char* __restrict format, ...)
{
    va_list args;
    unsigned args_printed = 0;

    va_start(args, format);
    while (*format != '\0')
    {
        args_printed += print(*format);
        format++;
    }

    return 0;
}