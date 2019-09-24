#include <stdio.h>

void outb(short port, char value)
{
    asm volatile ("outb %1, %0" : : "dN" (port), "a" (value));
}

char inb(short port)
{
   char ret;
   asm volatile("inb %1, %0" : "=a" (ret) : "dN" (port));
   return ret;
}

// short inw(short port)
// {
//    char ret;
//    asm volatile ("inw %1, %0" : "=a" (ret) : "dN" (port));
//    return ret;
// }