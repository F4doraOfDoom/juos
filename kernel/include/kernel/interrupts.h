/**
 * @file interrupts.h
 * @author Jonathan Uklisty (F4doraOfDoom) (yoniu7@gmail.com)
 * @brief Header file containing functions and structs related to interrupts
 * in the kernel
 * @version 0.1
 * @date 01-11-2019
 * 
 * @copyright Copyright Jonathan Uklisty (c) 2019 (MIT License)
 * 
 */

#ifndef KERNEL_INTERRUPTS_H_
#define KERNEL_INTERRUPTS_H_

#include <stdint.h>
#include "kstructs.h"

NAMESPACE_BEGIN(interrupts)

    typedef void (*handler)(void*);

    void initialize();
 
    void set_handler(uint32_t interrupt_num, handler handler);

NAMESPACE_END(interrupts)

#endif //KERNEL_INTERRUPTS_H_