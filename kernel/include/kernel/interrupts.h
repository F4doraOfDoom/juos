#ifndef KERNEL_INTERRUPTS_H_
#define KERNEL_INTERRUPTS_H_

#include <stdint.h>
#include "kstructs.h"

NAMESPACE_BEGIN(interrupts)

    typedef void (*handler)(void*);

    void initialize();
 
    void set_handler(uint32_t interrupt_num, handler handler);

NAMESPACE_END(interrupts)

#endif //KERNEL_INTERRUPTS_H_