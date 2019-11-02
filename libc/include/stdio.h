#ifndef _STDIO_H
#define _STDIO_H 1

// #include <sys/cdefs.h>
#include <sys/cdefs.h>

#define EOF (-1)

#ifdef __cplusplus
extern "C" {
#endif

#define PTR_PREFIX "0x"

__NO_MANGELING int printf(const char* __restrict, ...);
__NO_MANGELING int putchar(int);
__NO_MANGELING int puts(const char*);

#ifdef __cplusplus
}
#endif

#endif
