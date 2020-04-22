#include <kernel/schedualer.h>

using namespace kernel::scheduler;

// static void RestoreRegisters(RegistersStruct_x86_32* registers)
// {
//     asm ("push %0\n"
//          "popa" :: "r"(*registers));
// }

static KernelProcess* _CurrentProcess = nullptr;

KernelProcess* scheduler::GetCurrentProcess()
{
    return _CurrentProcess;
}

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

KernelProcess* ProcessScheduler::GetNext()
{
    return _CurrentProcess;
}

KernelProcess* ProcessScheduler::_FindProcess(Queue<KernelProcess*>& queue)
{
    KernelProcess* next_process = nullptr;

    while (queue.isempty() == false)
    {
        auto proc = queue.top();

        if (_IsCanceled(proc->pid))
        {
            queue.dequeue();
            //delete canceled;
            continue;
        }

        next_process = queue.dequeue();
        next_process->times_ran++;
        queue.enqueue(next_process);
        break;
    }

    return next_process;
}  

void ProcessScheduler::CalculateNext(RegistersStruct_x86_32* regs, void* args)
{
    KernelProcess* next_process = nullptr;

    next_process = _FindProcess(_system_processes);

    if (next_process == nullptr)
    {
        next_process = _FindProcess(_regular_processes);
    }

    _CurrentProcess = next_process;
}

void ProcessScheduler::SignalEnd(uint32_t pid)
{
    // implement some end
#ifdef K_LOG_SCHEDULER
    LOG_SA("SCHEDULER: ", "Finished running process %d\n", pid);
#endif

    _canceled_processes.push_back(pid);
}

void scheduler::run_process_scheduler(RegistersStruct_x86_32* regs, void* args)
{
    auto scheduler = static_cast<ProcessScheduler*>(args);
    // currently args = nullptr becuase we're not passing any args
    scheduler->CalculateNext(regs, nullptr);   
}
