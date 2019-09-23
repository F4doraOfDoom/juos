#include <stdio.h>
#include <stdarg.h>
#include <string.h>

static void print_s(const char* s, size_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        putchar(s[i]);
    }
} 

int printf(const char* __restrict format, ...)
{
    va_list args;
    unsigned args_printed = 0;

    va_start(args, format);
    while (*format != '\0')
    {
        bool printed_special = true;

        if (*format == '%')
        {   
            if (format[1] == '\0') 
            {
                break; 
            }
            else if (format[1] == 'c') 
            {
                putchar(va_arg(args, int));
            }
            else if (format[1] == 's')
            {
                const char* s = va_arg(args, const char*);
                print_s(s, strlen(s));
            }
            else
            {
                putchar('%');
                putchar((format[1]));
                printed_special = false;
            }
            
            format++;
        }
        else
        {
            putchar(*format);
            printed_special = false;
        }
        
        args_printed += printed_special;
        format++;
    }

    va_end(args);
    return args_printed;
}