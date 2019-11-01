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