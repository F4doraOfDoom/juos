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
#include <kernel/kstructs.h>

#include <kernel/scheduler_interface.h>
#include <kernel/keyboard.h>
#include <kernel/paging.h>
#include <kernel/lock.h>
#include <kernel/kmm.h>

#include <kernel/data_structures/string.hpp>
#include <kernel/data_structures/vector.hpp>
#include <kernel/data_structures/queue.hpp>

#include <move.hpp>
#include <algorithms.hpp>


using kernel::data_structures::Vector;
using kernel::data_structures::Queue;

#define KERNEL_STACK_BEGIN      0xA0000000
#define KERNEL_STACK_SIZE       0x01000000

NAMESPACE_BEGIN(kernel)

    NAMESPACE_BEGIN(Processing)

        template <typename T>
        struct InterruptGuard
        {
            InterruptGuard() {
                DISABLE_HARDWARE_INTERRUPTS();
            }

            InterruptGuard(T&& func) {
                DISABLE_HARDWARE_INTERRUPTS();
                func();
            }

            ~InterruptGuard()
            {
                ENABLE_HARDWARE_INTERRUPTS();
            }
        };

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

        struct Context
        {
            uint32_t eip, esp, ebp;
            paging::PageDirectory directory;    
        };
        

        struct KernelProcess
        {
            using ID        = uint32_t;
            // function to be called after the process ends
            // recieves the process itself, and arguments
            using ProcessResolver = void (*)(ID proc_id, void*); 

            enum class Priority
            {
                Low,
                Regular,
                High,
                Critical,
                System
            };

            // implemented in processing.cpp
            KernelProcess(const void* func_ptr, Priority priority);
            ~KernelProcess();

            /**
             * @brief Depracated 
             * 
             * @param context 
             */
            void ApplyContext(const Context* context);

        
            ID                              pid = (_pid_seq++);
            uint64_t                        start_time = 0;
            uint64_t                        slice_size;
            Priority                        priority;
            kernel::paging::PageDirectory*  directory = nullptr;
            // stack and heap pointers
            void*                           stack_begin = nullptr;
            void*                           heap_begin = nullptr;
            void*                           data_begin = nullptr;
            // counter for how many times the process has been task switched
            uint64_t                        times_ran = 0;
 
            #if 1 // implement some way to detect architecture
            RegistersStruct_x86_32          registers;
            bool                            registers_set = false;
            #endif 

            // needed to release pages later when process is over
            Vector<paging::_HeapMappingSettings>* self_mappings;

            // function that is run on the end of the process
            ProcessResolver on_end = nullptr; 

            // in contrast with other propreties, the input buffer will be part of 
            // of the processes's address space
            Queue<keyboard::InputKeyType>* input_buffer = nullptr; 
            DECLARE_LOCK(input_buffer_lock);

        private:
            bool        _is_finished; 
            static ID   _pid_seq;
        };

        struct CachedProcessInfo
        {
            KernelProcess::ID               pid;
            Queue<keyboard::InputKeyType>** input_buffer_ptr = nullptr;
            volatile int*                   input_buffer_lock_ptr = nullptr;
        };


        /**
         * @brief Add a process to the known processes list
         * 
         * @param name - name of the process
         * @param func - pointer to the process code
         */
        void RegisterProcess(const String& name, const void* func);

        /**
         * @brief Get the Current execution context
         * 
         * @param context 
         */
        inline void GetCurrentContext(Context* context)
        {
            asm volatile("mov %%esp, %0": "=r"(context->esp));
            asm volatile("mov %%ebp, %0": "=r"(context->ebp));
            memcpy(&context->directory, paging_current_directory, sizeof(paging::PageDirectory));
        }

        /**
         * @brief Get the Pid of the current running processs
         * 
         * @return KernelProcess::ID 
         */
        KernelProcess::ID GetPid();

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
            KernelProcess::ID Process(const String& name, KernelProcess::Priority priority);

        NAMESPACE_END(Start)
            
        NAMESPACE_BEGIN(End)

            /**
             * @brief Terminate a task with id _id_
             * 
             * @param id 
             */
            void Process(KernelProcess::ID id);

        NAMESPACE_END(End)
        

        using ProcessScheduler = scheduler::IScheduler<KernelProcess>*;
        using SchedulerCallback = void (*)(RegistersStruct_x86_32*, void*);
        using KernelStart = void (*)();
        /**
         * @brief Initializes the paging system
         * 
         * @param scheduler a pointer to an object that implements the IScheduler interface
         */
        void Initialize(KernelStart start, SchedulerCallback, ProcessScheduler scheduler);

        /**
         * @brief Get the Scheduler object
         * 
         * @return ProcessScheduler 
         */
        ProcessScheduler GetScheduler();

        /**
         * @brief Get the Input Buffer of the current process
         * 
         * @return Queue<keyboard::InputKeyType>* 
         */
        Queue<keyboard::InputKeyType>* GetInputBuffer();

        /**
         * @brief Locks the current processes's input buffer with a mutex
         * 
         */
        void LockInputBuffer();


        /**
         * @brief Unlocks the current processes's input buffer with a mutex
         * 
         */
        void UnlockInputBuffer();

        /**
         * @brief Initiates the process, from it's own address space perspective.
         * 
         */
        void SelfProcessInit();

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