#ifndef KERNEL_ISR_I386
#define KERNEL_ISR_I386

#include "descriptor_tables_structs.h"

__NO_MANGELING void isr_handler(Idt::registers_t);

#endif //KERNEL_ISR_I386