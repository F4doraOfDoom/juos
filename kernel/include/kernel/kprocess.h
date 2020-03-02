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

        struct RegisteredProcess
        {
            static constexpr uint32_t MaxNameLength = 32;

            char            name[MaxNameLength] = { 0 };
            const void*     func_ptr; // pointer to some code
        };

        struct Task
        {
            using ID = uint32_t;
            
            enum class Priority
            {
                Low,
                Regular,
                High,
                System
            };

            ID          tid;
            uint32_t    start_time;
            uint32_t    slice_size;
            Priority    priority;
        };

        /**
         * @brief Add a process to the known processes list
         * 
         * @param name - name of the process
         * @param func - pointer to the process code
         */
        void register_process(const char* name, const void* func);

        NAMESPACE_BEGIN(start)

            /**
             * @brief This function starts executing code from _code_ptr_
             * 
             * @param code_ptr - pointer to code
             */
            void code(const void* code_ptr);

            /**
             * @brief This function starts a process with name _name_
             * @param name 
             * @return Task::TaskID - id of the task
             */
            Task::ID process(const char* name);

        NAMESPACE_END(start)
            
        NAMESPACE_BEGIN(end)

            /**
             * @brief Terminate a task with id _id_
             * 
             * @param id 
             */
            void process(Task::ID id);

        NAMESPACE_END(end)
        

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