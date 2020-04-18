#ifndef KERNEL_IO_H
#define KERNEL_IO_H

#include <kernel/kdef.h>
#include <kernel/lock.h>

#include <stdint.h>
#include <stdio.h>

static DECLARE_LOCK(__print_lock);

#define LOCKED_PRINTF(fmt) \
    LOCK(__print_lock); \
    printf(fmt); \
    UNLOCK(__print_lock);


#define LOCKED_PRINTF_ARGS(fmt, ...) \
    LOCK(__print_lock); \
    printf(fmt, __VA_ARGS__); \
    UNLOCK(__print_lock);

#define SYNCED_PRINTF(fmt) \
    asm volatile("cli;"); \
    printf(fmt); \
    asm volatile("sti;"); \


#define SYNCED_PRINTF_ARGS(fmt, ...) \
    asm volatile("cli;"); \
    printf(fmt, __VA_ARGS__); \
    asm volatile("sti;"); \

NAMESPACE_BEGIN(kernel)

    NAMESPACE_BEGIN(IO)

        char GetChar();

        void GetString(char* buffer, uint32_t max_len);

    NAMESPACE_END(IO)

NAMESPACE_END(kernel)

#endif //