/**
 * @file dtables.h
 * @author Jonathan Uklisty (F4doraOfDoom) (yoniu7@gmail.com)
 * @brief Header file containing functions and operations related to 
 * descriptor tables in the kernel (i.e. GDT and IDT)
 * @version 0.1
 * @date 01-11-2019
 * 
 * @copyright Copyright Jonathan Uklisty (c) 2019 (MIT License)
 * 
 */

#ifndef KERNEL_DESCRIPTOR_TABLES_H
#define KERNEL_DESCRIPTOR_TABLES_H

#include <kernel/kdef.h>
#include <kernel/klog.h>

NAMESPACE_BEGIN(Gdt)

    /*
        This function is used to initilalize the global descriptor table
    */
    void Initialize();

NAMESPACE_END(Gdt)


NAMESPACE_BEGIN(Idt)

    /*
        This fucntion is used to Initialize the interrupt descriptor table
    */
    void Initialize();

NAMESPACE_END(Idt)

#endif //KERNEL_DESCRIPTORS_H