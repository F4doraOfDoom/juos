#ifndef KERNEL_ISR_I386
#define KERNEL_ISR_I386

#include "dtables.h"
#include "dtables_structs.h"

__NO_MANGELING void isr_handler(idt::registers_t);

#endif //KERNEL_ISR_I386