#include <kernel/schedualer.h>

using namespace kernel::scheduler;

// static void RestoreRegisters(RegistersStruct_x86_32* registers)
// {
//     asm ("push %0\n"
//          "popa" :: "r"(*registers));
// }

static KernelProcess* _CurrentProcess = nullptr;

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

void ProcessScheduler::Run(RegistersStruct_x86_32* regs, void* args)
{
    // usually we use a linked list in order to manage process list.
    // since i use a queue, we need to pop it and push it back
    auto run_and_set_back = [&](auto& queue) {
        auto proc = queue.dequeue();
        _ExecuteProcess(proc);
        proc->times_ran++;

        if (!proc->IsFinished())
        {
            queue.enqueue(proc);
        }
    };

    constexpr unsigned SERVE_SYSTEM_THRESHOLD = 5;
    static unsigned int served_system = 0;

    if (_keep_running == false)
    {
        return;
    }

    _keep_running = true;

    // update current process registers
    _CurrentProcess->registers = *regs;

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
}

__NO_MANGELING void _SetDirectory()
{
    paging::PageDirectory* directory = nullptr;

    asm volatile("mov %%eax, %0"::"r"(directory));

    paging::SetDirectory(directory);
}

static struct {
    uint32_t eip, esp, ebp;
    paging::PageDirectory directory;
} _temp_saved_data;

static uint32_t _ContextSwitch(KernelProcess* process)
{
    _temp_saved_data.eip = process->registers.eip;
    _temp_saved_data.esp = process->registers.esp;
    _temp_saved_data.ebp = process->registers.ebp;
    _temp_saved_data.directory = *process->directory;

    //memcpy(paging::current_directory, &_temp_dir, sizeof(paging::PageDirectory));
    paging::SetDirectory(&_temp_saved_data.directory);

    asm volatile("         \
        cli;                 \
        mov %0, %%ecx;       \
        mov %1, %%esp;       \
        mov %2, %%ebp;       " :: "r"(_temp_saved_data.eip), "r"(_temp_saved_data.esp), "r"(_temp_saved_data.ebp));

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

void scheduler::run_process_scheduler(RegistersStruct_x86_32* regs, void* args)
{
    // // save the last registers
    // if (_CurrentProcess)
    // {
    //     _CurrentProcess->registers_set = true;
    //     memcpy(&_CurrentProcess->registers, regs, sizeof(_CurrentProcess->registers));
    // }

    DisableHardwareInterrupts();
    
    // switch back to kernel address space
    paging::SetDirectory(paging::GetKernelDirectory());

    auto scheduler = static_cast<ProcessScheduler*>(args);
    // currently args = nullptr becuase we're not passing any args
    scheduler->Run(regs, nullptr);

    EnableHardwareInterrupts();
}
