#ifndef KERNEL_DESCRIPTORS_H
#define KERNEL_DESCRIPTORS_H

#include <kernel/kdef.h>

NAMESPACE_BEGIN(Gdt)

    /*
        This function is used to initilalize the global descriptor table
    */
    void initialize();

NAMESPACE_END(Gdt)

#endif //KERNEL_DESCRIPTORS_H