#ifndef KERNEL_DESCRIPTOR_TABLES_H
#define KERNEL_DESCRIPTOR_TABLES_H

#include <kernel/kdef.h>

NAMESPACE_BEGIN(gdt)

    /*
        This function is used to initilalize the global descriptor table
    */
    void initialize();

NAMESPACE_END(gdt)

NAMESPACE_BEGIN(idt)

    /*
        This fucntion is used to initialize the interrupt descriptor table
    */
    void initialize();

NAMESPACE_END(idt)

#endif //KERNEL_DESCRIPTORS_H