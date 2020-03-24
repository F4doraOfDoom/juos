#include <kernel/schedualer.h>


void ProcessScheduler::AddProcess(Process* process) 
{
    if (process->priority == Process::Priority::System)
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
    static unsigned served_system = 0;
    _keep_running = true;

    if (_keep_running == false)
    {
        return;
    }

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

void ProcessScheduler::_ExecuteProcess(Process* process)
{

}