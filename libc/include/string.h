#ifndef _STRING_H
#define _STRING_H 1

#include <cdefs.h>

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void* memset(void*, int, size_t);
size_t strlen(const char*);

#ifdef __cplusplus
}
#endif

#endif
