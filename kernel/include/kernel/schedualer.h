#ifndef KERNEL_PROC_SCHEDUALER_H
#define KERNEL_PROC_SCHEDUALER_H

#include <kernel/processing.h>
#include <kernel/scheduler_interface.h>
#include <kernel/data_structures/queue.hpp>

using namespace kernel;
using data_structures::Queue;
using Processing::KernelProcess;

NAMESPACE_BEGIN(kernel)

    NAMESPACE_BEGIN(scheduler)

        struct ProcessNode
        {
            ProcessNode*           next        = nullptr;
            KernelProcess*  proccess    = nullptr;
        };
        class ProcessScheduler : public IScheduler<KernelProcess>
        {
        public:
            ProcessScheduler()  {}  
  
            /**
             * @brief Add a process to the queue
             * 
             * @param process 
             */
            virtual void AddItem(KernelProcess* process);

            virtual KernelProcess* GetNext();

            virtual void CalculateNext(RegistersStruct_x86_32* regs, void* args);

            virtual void SignalEnd(uint32_t pid);

            /**
             * @brief Make the schedualer stop
             * 
             * @param args 
             */
            void Stop(void* args);


        private:
            bool _IsCanceled(KernelProcess::ID proc_id)
            {
                uint32_t index = 0;
                for (auto canceled_id : _canceled_processes)
                {
                    if (proc_id == canceled_id)
                    {
                        // TODO: Implement vector erase
                        _canceled_processes[index] = -1;
                        return true;
                    }
                    index++;
                }
                return false;
            }

            // used to keep track of processes
            

            /**
             * @brief This function executes a process
             * 
             * @param process 
             */
            void _ExecuteProcess(KernelProcess* process);

            Vector<KernelProcess::ID> _canceled_processes; 

            bool _keep_running = true;
        };

        void SwitchTask(void* regs);

        KernelProcess* GetCurrentProcess();

        void run_process_scheduler(RegistersStruct_x86_32* regs, void* args);
        
    NAMESPACE_END(scheduler)

NAMESPACE_END(kernel)

__NO_MANGELING uint32_t get_ip();

#endif // KERNEL_SCHEDUALER_H