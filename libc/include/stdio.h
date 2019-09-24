#ifndef _STDIO_H
#define _STDIO_H 1

// #include <sys/cdefs.h>
#include <sys/cdefs.h>

#define EOF (-1)

#ifdef __cplusplus
extern "C" {
#endif

__NO_MANGELING int printf(const char* __restrict, ...);
__NO_MANGELING int putchar(int);
__NO_MANGELING int puts(const char*);

/*
    Common port IO tasks
*/
__NO_MANGELING void     outb(short port, char value);
__NO_MANGELING char     inb(short port);
__NO_MANGELING short    inw(short port);

#ifdef __cplusplus
}
#endif

#endif
