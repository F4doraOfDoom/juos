#ifndef KERNEL_PROC_SCHEDUALER_H
#define KERNEL_PROC_SCHEDUALER_H

#include <kernel/kprocess.h>

#include <kernel/data_structures/queue.hpp>

using namespace kernel;
using data_structures::Queue;
using processing::Process;

class ProcessScheduler
{
public:
    ProcessScheduler& GetInstance()
    {
        static ProcessScheduler ps;
        
        return ps;
    }

    /**
     * @brief Add a process to the queue
     * 
     * @param process 
     */
    void AddProcess(Process* process);

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

    ProcessScheduler() {}  

    Queue<Process*> _system_processes;
    Queue<Process*> _regular_processes;

    bool _keep_running = true;
};

#endif // KERNEL_SCHEDUALER_H