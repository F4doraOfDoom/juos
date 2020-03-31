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
#include <kernel/kmm.h>
#include <kernel/scheduler_interface.h>

#include <kernel/paging.h>
#include <kernel/data_structures/vector.hpp>
#include <kernel/data_structures/string.hpp>

#include <move.hpp>
#include <algorithms.hpp>

#include <kernel/kstructs.h>

using kernel::data_structures::Vector;

NAMESPACE_BEGIN(kernel)

    NAMESPACE_BEGIN(Processing)

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

        struct KernelProcess
        {
            using ID        = uint64_t;

            enum class Priority
            {
                Low,
                Regular,
                High,
                Critical,
                System
            };

            KernelProcess(const void* func_ptr, Priority priority)
            {
                active_threads.push_back(Thread{func_ptr});
                this->priority = priority;

                // the time each process gets to run is equal to
                // its priority on a scale of 1-5, times 10
                slice_size = ((uint64_t)priority + 1) * 10;
            }

            ID                              pid = (_pid_seq++);
            uint64_t                        start_time = Timer::current_time();
            uint64_t                        slice_size;
            Priority                        priority;
            Vector<Thread>                  active_threads;
            kernel::paging::PageDirectory*  directory = nullptr;
            // stack and heap pointers
            void*                           stack_begin = nullptr;
            void*                           heap_begin = nullptr;
            void*                           data_begin = nullptr;
            #if 1 // implement some way to detect architecture
            RegistersStruct_x86_32          registers;
            bool                            registers_set = false;
            #endif 

        private:
            static ID _pid_seq;
        };

        /**
         * @brief Add a process to the known processes list
         * 
         * @param name - name of the process
         * @param func - pointer to the process code
         */
        void RegisterProcess(const String& name, const void* func);

        NAMESPACE_BEGIN(Start)

            /**
             * @brief This function starts executing code from _code_ptr_
             * 
             * @param code_ptr - pointer to code
             */
            void Code(const void* code_ptr);

            /**
             * @brief This function starts a process with name _name_
             * @param name 
             * @return observer pointer to the process struct
             */
            const KernelProcess* Process(const String& name, KernelProcess::Priority priority);

        NAMESPACE_END(Start)
            
        NAMESPACE_BEGIN(end)

            /**
             * @brief Terminate a task with id _id_
             * 
             * @param id 
             */
            void Process(KernelProcess::ID id);

        NAMESPACE_END(end)
        

        using ProcessScheduler = scheduler::IScheduler<KernelProcess>*;
        /**
         * @brief Initializes the paging system
         * 
         * @param scheduler a pointer to an object that implements the IScheduler interface
         */
        void Initialize(ProcessScheduler scheduler);

    NAMESPACE_END(Processing)

NAMESPACE_END(kernel)

/**
 * @brief Set the current instruction pointer to _ptr_
 * 
 * @param ptr 
 * @param eip 
 */
__NO_MANGELING void _set_instruction_ptr(uint32_t* ptr);

#endif // KERNEL_PROCESS_H_