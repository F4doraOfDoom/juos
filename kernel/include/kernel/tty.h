#ifndef KERNEL_TTY_H
#define KERNEL_TTY_H
 
#include <stddef.h>
#include <kernel/kdef.h> 

NAMESPACE_BEGIN(Tty)

void initialize(void);
void putchar(char c);
void write(const char* data, size_t size);
void writestring(const char* data);
 
NAMESPACE_END(Tty)

#endif