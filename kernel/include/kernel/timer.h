/**
 * @file timer.h
 * @author Jonathan Uklisty (F4doraOfDoom) (yoniu7@gmail.com)
 * @brief Header file containing all time related functions and structs
 * in the kernel 
 * @version 0.1
 * @date 01-11-2019
 * 
 * @copyright Copyright Jonathan Uklisty (c) 2019 (MIT License)
 * 
 */

#ifndef KERNEL_TIMER_H_
#define KERNEL_TIMER_H_

#include <stdint.h>
#include <stdio.h>

#include "interrupts.h"
#include "kdef.h"

// the hardware clock will call this interrupt on every tick
#define TIMER_PORT 32

NAMESPACE_BEGIN(timer)

    /**
     * @brief this function initializes the clock, implemented by arch
     * 
     * @param clock_freq - the frequency of the clock's updates
     */
    void initialize(uint32_t clock_freq);

    /**
     * @brief this function calls the arch's implenetation of "initialize",
     * and sets up the kernel internal clock (software instead of arch) 
     * 
     * @param clock_freq - the frequency of the clock's updates
     */
    void start(uint32_t clock_freq);


    /**
     * @brief this function is called upon every tick. it updates the kernel's
     * internal tick count
     * 
     */
    static void __tick_handler(void*);

    // internal tick counter 
    static uint64_t __tick_counter = 0;

NAMESPACE_END(timer)

#endif //KERNEL_TIMER_H_