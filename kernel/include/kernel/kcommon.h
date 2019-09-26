#ifndef KERNEL_COMMON_H_
#define KERNEL_COMMON_H_

#include "kdef.h"

/*
    Common port IO tasks
*/
__NO_MANGELING inline void outb(short port, char value) {
    asm volatile ("outb %1, %0" : : "dN" (port), "a" (value));
}

__NO_MANGELING inline char inb(short port) {
   char ret;
   asm volatile("inb %1, %0" : "=a" (ret) : "dN" (port));
   return ret;
}

#endif // KERNEL_COMMON_H_