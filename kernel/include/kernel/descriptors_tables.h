#ifndef KERNEL_DESCRIPTOR_TABLES_H
#define KERNEL_DESCRIPTOR_TABLES_H

#include <kernel/kdef.h>



NAMESPACE_BEGIN(Gdt)

    /*
        This function is used to initilalize the global descriptor table
    */
    void initialize();

NAMESPACE_END(Gdt)

NAMESPACE_BEGIN(Idt)

    /*
        This fucntion is used to initialize the interrupt descriptor table
    */
    void initialize();

NAMESPACE_END(Idt)

#endif //KERNEL_DESCRIPTORS_H