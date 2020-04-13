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

void ProcessScheduler::CalculateNext(RegistersStruct_x86_32* regs, void* args)
{
    KernelProcess* next_process = nullptr;

    // usually we use a linked list in order to manage process list.
    // since i use a queue, we need to pop it and push it back
    auto run_and_set_back = [&](auto& queue) {
        
        if (!_IsCanceled(queue.top()->pid)) 
        {
            auto proc = queue.dequeue();
#if defined(K_LOG_SCHEDULER)
        LOG_SA("SCHEDULER: ", "Next process: %d\n", proc->pid);
#endif
            next_process = proc;
            _CurrentProcess = proc;
            proc->times_ran++;
            queue.enqueue(proc);
        }
    };

    constexpr unsigned SERVE_SYSTEM_THRESHOLD = 1;
    static unsigned int served_system = 0;

    if (_keep_running == false)
    {
        _CurrentProcess = nullptr;
        return;
    }

    _keep_running = true;

    // update current process registers
    //_CurrentProcess->registers = *regs;

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
        run_and_set_back(_system_processes);

        served_system++;
    }
    else 
    {
        if (!_regular_processes.isempty())
        {
            run_and_set_back(_regular_processes);
        }
        else
        {
            _CurrentProcess = nullptr;
        } 

        served_system = 0;
    }

    _CurrentProcess = next_process;
}

// we're using a global variable to store the context
// becase its mapped in all address spaces
Processing::Context _temp_context;
Processing::Context _last_context;

static uint32_t _ContextSwitch(KernelProcess* process)
{
    // saving the context of the last running process
    Processing::GetCurrentContext(&_last_context);

    _temp_context.eip = process->registers.eip;
    _temp_context.esp = process->registers.esp;
    _temp_context.ebp = process->registers.ebp;
    _temp_context.directory = *process->directory;

    //memcpy(paging::current_directory, &_temp_dir, sizeof(paging::PageDirectory));
    paging::SetDirectory(&_temp_context.directory);

    asm volatile("         \
        cli;                 \
        mov %0, %%ecx;       \
        mov %1, %%esp;       \
        mov %2, %%ebp;       " :: "r"(_temp_context.eip), "r"(_temp_context.esp), "r"(_temp_context.ebp));

    asm volatile("\
        mov $0x12345, %eax; \
        sti;                 \
        jmp *%ecx           "); 
   
    return 0;
}

void ProcessScheduler::_ExecuteProcess(KernelProcess* process)
{
#ifdef K_LOG_SCHEDULER
    LOG_SA("SCHEDULER:", "Serving process %d\n", process->pid)
#endif
    _CurrentProcess = process;

    //RegistersStruct_x86_32 saved_regs = process->registers;

    _ContextSwitch(process);
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
    // // save the last registers
    // if (_CurrentProcess)
    // {
    //     _CurrentProcess->registers_set = true;
    //     memcpy(&_CurrentProcess->registers, regs, sizeof(_CurrentProcess->registers));
    // }

    // DisableHardwareInterrupts();
    
    // // switch back to kernel address space
    // paging::SetDirectory(paging::GetKernelDirectory());

    auto scheduler = static_cast<ProcessScheduler*>(args);
    // currently args = nullptr becuase we're not passing any args
    scheduler->CalculateNext(regs, nullptr);

    // EnableHardwareInterrupts();

    
}
