#ifndef KERNEL_INTERRUPTS_H_
#define KERNEL_INTERRUPTS_H_

#include <stdint.h>

typedef void (*interrupt_handler)(void*);

void set_interrupt_handler(uint32_t interrupt_num, interrupt_handler handler);

#endif //KERNEL_INTERRUPTS_H_