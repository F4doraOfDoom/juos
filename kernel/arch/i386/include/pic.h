#ifndef KERNEL_PIC_I386
#define KERNEL_PIC_I386

#include <stdint.h>

#include <kernel/kdef.h>

#include "interrupts.h"

NAMESPACE_BEGIN(pic)

    /*
        Initializes the PIC (Programmable Interrupt Controller)
        uint32_t base - at what interrupt index do you want the PIC to begin?
            (Example: A base of 32 will map the PIC interrupts from 32 - 47) 
    */
    void initialize(uint32_t base);

    /*
        Enables devices in the PIC
        uint32_t master_dev - devices to be enabled in the master PIC
        uint32_t slave_dev - devices to be enabled in the slave PIC
    */
    static inline void _enable_devices(uint8_t master_dev, uint8_t slave_dev);

    /*
        Maps the PIC to certain offsets in the interrupt table
        uint8_t master_base - the first address that the master PIc will be mapped to in 
            the interrupt table
        uint8_5 slave_base - same but for the base
    */
    static inline void _pic_remap(uint8_t master_base, uint8_t slave_base);  

    static inline void _add_to_int_table();  

NAMESPACE_END(pic)

// will be written in a seperate .S file
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

#endif //KERNEL_PIC_I386