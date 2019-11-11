#include <stdio.h>
#include <stdarg.h>
#include <string.h>


#ifdef __is_libk
    #include <kernel/tty.h>
    #include <kernel/klog.h>
    extern void tty::set_color(unsigned int fg, unsigned int bg);
#else
    // do sys calls?
#endif

/**
 * @brief Converts _value_ of type _ValT_ to string with
 * numerical representation of _base_
 * 
 * @tparam ValT - type of the value. Should be unsigned, whole type 
 * @tparam MaxStrSize - largest possible digits in number
 * @param base - nueric base of convertion. Should not be bigger than 16!
 * @param value - value to be converted 
 * @return const char* 
 */
template <typename ValT, size_t MaxStrSize = 64>
static const char* _convert_to_base(ValT value, unsigned const int base = 10)
{
    static const char representation[] = "0123456789abcdef";
    static char buf[MaxStrSize];

    memset(buf, '\0', MaxStrSize);

    char* end = &buf[MaxStrSize - 1];
    
    do
    {
        *--end = representation[value%base];
        value /= base;
    } while (value != 0);
    
    return end;
}

/**
 * @brief print _len_ chars of string _s_
 * 
 * @param s - string to print
 * @param len - number of chars to print 
 */
static void print_s(const char* s, size_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        putchar(s[i]);
    }
} 

/**
 * @brief print a whole, unsigned numeric value
 * 
 * @tparam ValT - type of numeric value
 * @param n - numeric value to print
 * @param base - base of the number (between 2 and 16)
 */
template <typename ValT>
static inline void print_n(ValT n, unsigned const int base)
{
    const char* rep = _convert_to_base(n, base); 
    print_s(rep, strlen(rep));
}

int printf(const char* __restrict format, ...)
{
    va_list args;
    unsigned args_printed = 0;

    va_start(args, format);
    while (*format != '\0')
    {
        bool printed_special = true;

        // normal argument
        if (*format == '%')
        {   
            if (format[1] == '\0') 
            {
                break; 
            }

            switch (format[1])
            {
                // signle character
                case 'c':
                    putchar(va_arg(args, int));
                break;

                // null terminated string
                case 's':
                {
                    const char* s = va_arg(args, const char*);
                    print_s(s, strlen(s));
                }
                break;

                // print at base 10
                case 'd':
                    print_n(va_arg(args, int), 10);
                break;

                // print at base 16
                case 'x':
                    print_n(va_arg(args, int), 16);
                break;

                // print at base 8
                case 'o':
                    print_n(va_arg(args, int), 8);
                break;

                // print pointer at base 16
                case 'p':
                    print_s(PTR_PREFIX, strlen(PTR_PREFIX));
                    print_n(va_arg(args, int), 16);
                break;

                default:
                    putchar('%');
                    putchar((format[1]));
                    printed_special = false;
                break;
            }
            
            format++;
        }
#ifdef __is_libk
        // kernel specific special escaping
        else if (*format == '$')
        {
            switch (format[1])
            {
                // color escaping
                // there is not error checking, make sure you don't do anythin stupid
                // supports colors from 0x0 - 0xF
                // pretty shitty, but good enough for the kernel currently
                case '&':
                {
                    unsigned int fg = format[2];
                    unsigned int bg = format[3];

                    tty::set_color(fg, bg);

                    format += 3;
                }
                break;

                default:
                    putchar('$');
                    putchar((format[1]));
                    format++;
                break;
            }

        }
#endif
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