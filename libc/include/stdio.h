#ifndef _STDIO_H
#define _STDIO_H 1

// #include <sys/cdefs.h>
#include <sys/cdefs.h>

#define EOF (-1)

#ifdef __is_libk
    #define __PRINTF_SPECIAL_ESCAPE         "$"  
    #define __PRINTF_SPECIAL_ESCAPE_COLOR   "&"
    #define __PRINTF_COLOR_SEQUENCE         __PRINTF_SPECIAL_ESCAPE __PRINTF_SPECIAL_ESCAPE_COLOR
#endif

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
