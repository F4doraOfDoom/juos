#ifndef _STRING_H
#define _STRING_H 1

#include <stddef.h>
#include <sys/cdefs.h>

__NO_MANGELING void* memset(void*, int, size_t);
__NO_MANGELING void *memcpy(void *, const void *, size_t n);
__NO_MANGELING size_t strlen(const char*);
__NO_MANGELING int strcmp(const char*, char const*);
__NO_MANGELING char* strncpy(char* destination, const char* source, size_t num);

#endif
