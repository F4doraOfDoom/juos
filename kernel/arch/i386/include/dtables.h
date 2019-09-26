#ifndef KERNEL_DESCRIPTOR_TABLES_I386
#define KERNEL_DESCRIPTOR_TABLES_I386

#include <stdint.h>
#include <kernel/kdef.h>
#include <kernel/tty.h>
#include <kernel/interrupts.h>
#include <kernel/dtables.h>
#include <kernel/kcommon.h>

#include "interrupts.h"

static void _pic_remap(uint8_t offset1, uint8_t offset2);
static void _set_all_interrupts();

NAMESPACE_BEGIN(gdt)

    /*
        This function initializes the GDT table and writes it to the CPU
    */
    static void _init();

    /*
        This function configures the GDT
        Input:
            int32_t     entry   - The index of the GDT entry
            uint32_t    base    - The base address of the GDT entry
            uint32_t    limit   - The upper limit of the GDT entry
            Gsat        access  - Entry access flags
            uint8_t     gran    - granuary byte
    */
    static void _config_entry(int32_t entry, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);               


NAMESPACE_END(gdt)

NAMESPACE_BEGIN(idt)

    /*
        This function initializes the IDT table and writes it to the CPU
    */
    static void _init();

    /*
        This function configures an IDT entry by index
            int32_t entry - index of the IDT entry
            uint32_5 base - the base addr of the interrupt handler
            uint16_t sel - segment selector
            uint8_t flags - IDT flags
    */
    static void _config_entry(int32_t entry, uint32_t base, uint16_t sel, uint8_t flags);

    template <uint32_t N>
    static inline void _request_isr();

NAMESPACE_END(idt)

/*
    Written in a different assembly file.
    Writes a given GDT into the CPU.
*/
__NO_MANGELING void gdt_dump(uint32_t table);

/*
    Written in a different assembly file
    Writes a given IDT into the CPU
*/
__NO_MANGELING void idt_dump(uint32_t table);

/*
    These functions will be written in a basic_callers.S
*/
__NO_MANGELING void isr_0();
__NO_MANGELING void isr_1();
__NO_MANGELING void isr_2();
__NO_MANGELING void isr_3();
__NO_MANGELING void isr_4();
__NO_MANGELING void isr_5();
__NO_MANGELING void isr_6();
__NO_MANGELING void isr_7();
__NO_MANGELING void isr_8();
__NO_MANGELING void isr_9();
__NO_MANGELING void isr_10();
__NO_MANGELING void isr_11();
__NO_MANGELING void isr_12();
__NO_MANGELING void isr_13();
__NO_MANGELING void isr_14();
__NO_MANGELING void isr_15();
__NO_MANGELING void isr_16();
__NO_MANGELING void isr_17();
__NO_MANGELING void isr_18();
__NO_MANGELING void isr_19();
__NO_MANGELING void isr_20();
__NO_MANGELING void isr_21();
__NO_MANGELING void isr_22();
__NO_MANGELING void isr_23();
__NO_MANGELING void isr_24();
__NO_MANGELING void isr_25();
__NO_MANGELING void isr_26();
__NO_MANGELING void isr_27();
__NO_MANGELING void isr_28();
__NO_MANGELING void isr_29();
__NO_MANGELING void isr_30();
__NO_MANGELING void isr_31();

__NO_MANGELING void irq_0();
__NO_MANGELING void irq_1();
__NO_MANGELING void irq_2();
__NO_MANGELING void irq_3();
__NO_MANGELING void irq_4();
__NO_MANGELING void irq_5();
__NO_MANGELING void irq_6();
__NO_MANGELING void irq_7();
__NO_MANGELING void irq_8();
__NO_MANGELING void irq_9();
__NO_MANGELING void irq_10();
__NO_MANGELING void irq_11();
__NO_MANGELING void irq_12();
__NO_MANGELING void irq_13();
__NO_MANGELING void irq_14();
__NO_MANGELING void irq_15();

#endif //KERNEL_DESCRIPTOR_TABLES_I386