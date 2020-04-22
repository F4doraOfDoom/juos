#ifndef KERNEL_HARDWARE_H
#define KERNEL_HARDWARE_H

#include <kernel/kdef.h>

// implemented in arch
#define DISABLE_HARDWARE_INTERRUPTS() \
    asm volatile("cli;")

// implemented in arch
#define ENABLE_HARDWARE_INTERRUPTS() \
    asm volatile("sti;")

#endif // KERNEL_HARDWARE_H