#ifndef KERNEL_HARDWARE_H
#define KERNEL_HARDWARE_H

#include <kernel/kdef.h>

// implemented in arch
__NO_MANGELING void DisableHardwareInterrupts();

// implemented in arch
__NO_MANGELING void EnableHardwareInterrupts();

#endif // KERNEL_HARDWARE_H