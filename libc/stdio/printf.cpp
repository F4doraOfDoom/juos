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

// template <typename N>
// static void print_numeric(const N n)
// {

//     constexpr size_t s_len = sizeof(N); 
//     char s_rep[s_len] = { 0 };
//     N n_copy = n;
//     size_t i = 0, cur = 0;

//     for (; i < s_len; i++, cur=cur*10)
//     {
//         s_rep[i] = static_cast<int>((n_copy % cur) / cur);
//         putchar(s);
//     }
// }

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

            switch (format[1])
            {
                case 'c':
                    putchar(va_arg(args, int));
                break;

                case 's':
                {
                    const char* s = va_arg(args, const char*);
                    print_s(s, strlen(s));
                }
                break;

                default:
                    putchar('%');
                    putchar((format[1]));
                    printed_special = false;
                break;
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