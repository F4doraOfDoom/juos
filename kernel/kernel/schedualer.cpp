#include <kernel/schedualer.h>

using namespace kernel::scheduler;

void ProcessScheduler::AddItem(KernelProcess* process) 
{
    if (process->priority == KernelProcess::Priority::System)
    {
        _system_processes.enqueue(process);
    }
    else
    {
        _regular_processes.enqueue(process);
    }
}

void ProcessScheduler::Run(void* args)
{
    constexpr unsigned SERVE_SYSTEM_THRESHOLD = 5;
    static unsigned int served_system = 0;

    if (_keep_running == false)
    {
        return;
    }

    _keep_running = true;
    
    // are there any important processes to run?
    // we want to give a high priority of execution to system processes, 
    // but if we only serve system processes all of the time, then we 
    // can starve regular processes from execution time.
    // To prevent that, we set the _served_system_ counter. It gets incremented
    // each time we serve a system process, and if it passes the 
    // SERVE_SYSTEM_THRESHOLD, then we give some execution time to a regular
    // process and the counter gets reset.
    if (!_system_processes.isempty() && served_system < SERVE_SYSTEM_THRESHOLD)
    {
        _ExecuteProcess(_system_processes.dequeue());
        served_system++;
    }
    else 
    {
        if (!_regular_processes.isempty())
        {
            _ExecuteProcess(_regular_processes.dequeue());
        }

        served_system = 0;
    }
}

void ProcessScheduler::_ExecuteProcess(KernelProcess* process)
{
#ifdef K_LOG_SCHEDULER
    LOG_SA("SCHEDULER:", "Serving process %d\n", process->pid)
#endif
}

void scheduler::run_process_scheduler(void* args)
{
    auto scheduler = static_cast<ProcessScheduler*>(args);
    scheduler->Run(nullptr);
}
