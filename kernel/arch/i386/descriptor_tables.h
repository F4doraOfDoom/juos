#ifndef KERNEL_DESCRIPTOR_TABLES_I386
#define KERNEL_DESCRIPTOR_TABLES_I386

#include <stdint.h>
#include <kernel/kdef.h>
#include <kernel/tty.h>
#include <kernel/descriptors_tables.h>

NAMESPACE_BEGIN(Gdt)


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

NAMESPACE_END(Gdt)

NAMESPACE_BEGIN(Idt)

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
    static inline void _call_general_interrupt_handler();

NAMESPACE_END(Idt)

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
    This function handles interrupts, and dispatches them to the correct handler
*/
__NO_MANGELING void general_interrupt_handler();

#endif //KERNEL_DESCRIPTOR_TABLES_I386