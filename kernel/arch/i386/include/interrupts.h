#ifndef KERNEL_ISR_I386
#define KERNEL_ISR_I386

#include <stdio.h>

#include <kernel/kdef.h>
#include <kernel/kstructs.h>
#include <kernel/kuseful.h>
#include <kernel/kcommon.h>
#include <kernel/interrupts.h>

#include "dtables.h"
#include "dtables_structs.h"

#define IRQ_0 32
#define IRQ_1 33
#define IRQ_2 34
#define IRQ_3 35
#define IRQ_4 36
#define IRQ_5 37
#define IRQ_6 38
#define IRQ_7 39
#define IRQ_8 40
#define IRQ_9 41
#define IRQ_10 42
#define IRQ_11 43
#define IRQ_12 44
#define IRQ_13 45
#define IRQ_14 46
#define IRQ_15 47

// End of Interrupt
#define PIC_RESET 0x20
#define PIC_MASTER 0x20
#define PIC_SLAVE 0xA0

#define PIC_IS_SLAVE(num)   IN_RANGE_C(num, IRQ_8, IRQ_15)
#define PIC_IS_MASTER(num)  IN_RANGE_C(num, IRQ_0, IRQ_7)

#define I386_INTERRUPTS 256

extern interrupts::handler interrupt_handlers[I386_INTERRUPTS];

#define GO_TO_INTERRUPT(num) \
    if (interrupt_handlers[registers.int_no] != nullptr) { \
        interrupt_handlers[registers.int_no]((void*)&registers); \
    } \
    else { \
        printf("Interrupt %c without any handler!", num + 45); \
        GO_PANIC(); \
    }

typedef void (*isr_t)(registers32_t);

__NO_MANGELING void isr_handler(registers32_t);
__NO_MANGELING void irq_handler(registers32_t);

#endif //KERNEL_ISR_I386