#ifndef KERNEL_INTERRUPTS_H_
#define KERNEL_INTERRUPTS_H_

#include <stdint.h>

NAMESPACE_BEGIN(Interrupts)

    typedef void (*handler)(void*);
    
    struct HandlerEntry
    {
        bool is_set;
        handler ihandler;
    };

    void initialize();
 
    void set_handler(uint32_t interrupt_num, handler handler);

NAMESPACE_END(Interrupts)

#endif //KERNEL_INTERRUPTS_H_