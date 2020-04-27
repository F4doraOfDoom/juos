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

#include <kernel/data_structures/vector.hpp>
using kernel::data_structures::Vector;

#include <kernel/schedualer.h>
#include "klog.h"
#include "interrupts.h"
#include "kdef.h"

// the hardware clock will call this interrupt on every tick
#define TIMER_PORT 32

NAMESPACE_BEGIN(Timer)


    /**
     * @brief this function initializes the clock, implemented by arch
     * 
     * @param clock_freq - the frequency of the clock's updates
     */
    void Initialize(uint32_t clock_freq);

    /**
     * @brief this function calls the arch's implenetation of "Initialize",
     * and sets up the kernel internal clock (software instead of arch) 
     * 
     * @param clock_freq - the frequency of the clock's updates
     */
    void start(uint32_t clock_freq);

    /**
     * @brief Make the current thread sleep until n time slices have passed.
     * A time slice is defined by the clock frequency passed to timer::start 
     * 
     * TODO: Add multithread support
     * 
     * @param slices - n of time slices to sleep
     */
    void sleep(uint32_t slices);

    /**
     * @brief Get the current time
     * 
     * @return uint64_t 
     */
    uint64_t current_time();

    /**
     * @brief this function is called upon every tick. it updates the kernel's
     * internal tick count
     * 
     */
    void __tick_handler(void*);

    // first is registers, second is function args
    using CallableFunc = void (*)(RegistersStruct_x86_32*, void*);

    /**
     * @brief Add a function to be called every tick
     * 
     * @param func 
     */
    void add_callable_function(CallableFunc func, void* args);

    void Sleep(uint32_t cycles);

    // internal tick counter 
    // defined in timer.cpp
    extern uint64_t __tick_counter;
    

NAMESPACE_END(Timer)

#endif //KERNEL_TIMER_H_