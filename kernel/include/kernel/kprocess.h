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
#include <kernel/timer.h>

#include <kernel/data_structures/vector.hpp>
#include <kernel/data_structures/string.hpp>

#include <move.hpp>
#include <algorithms.hpp>

using kernel::data_structures::Vector;

NAMESPACE_BEGIN(kernel)

    NAMESPACE_BEGIN(processing)

        struct RegisteredProcess
        {
            RegisteredProcess() : name(""), func_ptr(nullptr) {}
            RegisteredProcess(const String& _name, const void* _ptr) : name(_name), func_ptr(_ptr) {}

            String          name;
            const void*     func_ptr; // pointer to some code

            void operator=(const RegisteredProcess& p)
            {
                this->name      = p.name;
                this->func_ptr  = p.func_ptr;
            }
        };

        struct Thread
        {
            const void* current_address;
        };

        struct Process
        {
            using ID        = uint64_t;

            enum class Priority
            {
                Low,
                Regular,
                High,
                System
            };

            Process(const void* func_ptr, Priority priority)
            {
                pid = (_pid_seq++);
                start_time = timer::current_time();
                active_threads.push_back(Thread{func_ptr});
            }

            ID              pid;
            uint64_t        start_time;
            uint64_t        slice_size;
            Priority        priority;
            Vector<Thread>  active_threads;

        private:
            static ID _pid_seq;
        };

        /**
         * @brief Add a process to the known processes list
         * 
         * @param name - name of the process
         * @param func - pointer to the process code
         */
        void register_process(const String& name, const void* func);

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
            Process* process(const String& name, Process::Priority priority);

        NAMESPACE_END(start)
            
        NAMESPACE_BEGIN(end)

            /**
             * @brief Terminate a task with id _id_
             * 
             * @param id 
             */
            void process(Process::ID id);

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