#include <stdio.h>

#ifdef __is_libk
    #include <kernel/tty.h>
    extern void tty::putchar(char c);
#endif 

int putchar(const int c)
{
    #ifdef __is_libk
        tty::putchar(c);
    #else
        // some syscall
        volatile char x = c;
        x++;
    #endif

    return 0;
}