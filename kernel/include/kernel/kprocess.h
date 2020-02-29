/**
 * @file process.h
 * @author your name (you@domain.com)
 * @brief Header file for things regarding processing and processes
 * @version 0.1
 * @date 2020-02-29
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef KERNEL_PROCESS_H_
#define KERNEL_PROCESS_H_

#include <stdint.h>

#include <kernel/kdef.h>

NAMESPACE_BEGIN(kernel)

    NAMESPACE_BEGIN(processing)

        struct Task
        {
            enum class Priority
            {
                Low,
                Regular,
                High,
                System
            };

            uint32_t    start_time;
            uint32_t    slice_size;
            Priority    priority;
        };

        /**
         * @brief This function starts executing code from _code_ptr_
         * 
         * @param code_ptr - pointer to code
         */
        void run_code(const void* code_ptr);

    NAMESPACE_END(processing)

NAMESPACE_END(kernel)

/**
 * @brief Set the current instruction pointer to _ptr_
 * 
 * @param ptr 
 * @param eip 
 */
__NO_MANGELING void _set_instruction_ptr(uint32_t* ptr);

#endif // KERNEL_PROCESS_H_