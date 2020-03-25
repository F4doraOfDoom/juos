#ifndef KERNEL_ISR_I386
#define KERNEL_ISR_I386

#include <stdio.h>

#include <kernel/kdef.h>
#include <kernel/kstructs.h>
#include <kernel/kuseful.h>
#include <kernel/kcommon.h>
#include <kernel/interrupts.h>
#include <kernel/klog.h>

#include "idt.h"

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

#define ICW1_ICW4	0x01		/* ICW4 (not) needed */
#define ICW1_SINGLE	0x02		/* Single (cascade) mode */
#define ICW1_INTERVAL4	0x04		/* Call address interval 4 (8) */
#define ICW1_LEVEL	0x08		/* Level triggered (edge) mode */
#define ICW1_INIT	0x10		/* Initialization - required! */
 
#define ICW4_8086	0x01		/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO	0x02		/* Auto (normal) EOI */
#define ICW4_BUF_SLAVE	0x08		/* Buffered mode/slave */
#define ICW4_BUF_MASTER	0x0C		/* Buffered mode/master */
#define ICW4_SFNM	0x10		/* Special fully nested (not) */

#define PIC1		0x20		/* IO base address for master PIC */
#define PIC2		0xA0		/* IO base address for slave PIC */
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	(PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	(PIC2+1)

#define PIC_ACK_MASTER()  outb(0x20,0x20);
#define PIC_ACK_SLAVE()   outb(0x20,0x20); outb(0xa0,0x20);

// End of Interrupt
#define PIC_RESET 0x20
#define PIC_MASTER 0x20
#define PIC_SLAVE 0xA0

#define PIC_IS_SLAVE(num)   IN_RANGE_C(num, IRQ_8, IRQ_15)
#define PIC_IS_MASTER(num)  IN_RANGE_C(num, IRQ_0, IRQ_7)

#define I386_INTERRUPTS 256

extern Interrupts::handler interrupt_handlers[I386_INTERRUPTS];

typedef void (*isr_t)(registers32_t);

NAMESPACE_BEGIN(Interrupts)

    /*
        A function that calls idt::edit_entry
    */
    void AddToIdt(int32_t entry, uint32_t base, uint16_t sel, uint8_t flags);

NAMESPACE_END(Interrupts)

__NO_MANGELING void isr_handler(registers32_t);
__NO_MANGELING void irq_handler(registers32_t);

#endif //KERNEL_ISR_I386