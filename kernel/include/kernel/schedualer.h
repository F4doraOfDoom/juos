#ifndef KERNEL_PROC_SCHEDUALER_H
#define KERNEL_PROC_SCHEDUALER_H

#include <kernel/kprocess.h>
#include <kernel/scheduler_interface.h>
#include <kernel/data_structures/queue.hpp>

using namespace kernel;
using data_structures::Queue;
using processing::Process;

NAMESPACE_BEGIN(kernel)

    NAMESPACE_BEGIN(scheduler)

        class ProcessScheduler : public IScheduler<Process>
        {
        public:
            ProcessScheduler() {}  
  
            /**
             * @brief Add a process to the queue
             * 
             * @param process 
             */
            void AddItem(Process* process);

            /**
             * @brief This function should pass to the timer to run every tick
             * 
             * @param args 
             */
            void Run(void* args);

            /**
             * @brief Make the schedualer stop
             * 
             * @param args 
             */
            void Stop(void* args);

        private:
            /**
             * @brief This function executes a process
             * 
             * @param process 
             */
            void _ExecuteProcess(Process* process);


            Queue<Process*> _system_processes;
            Queue<Process*> _regular_processes;

            bool _keep_running = true;
        };

        void run_process_scheduler(void* args);
        
    NAMESPACE_END(scheduler)

NAMESPACE_END(kernel)

#endif // KERNEL_SCHEDUALER_H