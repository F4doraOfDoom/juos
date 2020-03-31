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
        else
        {
            _CurrentProcess = nullptr;
        } 

        served_system = 0;
    }
}

static uint32_t _ContextSwitch(KernelProcess* process)
{
    // save the previous execution point of the process for restoration
    uint32_t saved_ip = process->registers.eip;

    // set the execution point to the current line
    process->registers.eip = (uint32_t)__builtin_return_address(0);

    // restore process registers...
    //asm volatile("push %0; pop %%ds" :"=r"(process->registers.ds)); // data segment
    asm volatile("mov %0, %%ebx" :"=r"(process->registers.ebx));
    asm volatile("mov %0, %%edx" :"=r"(process->registers.edx));
    asm volatile("mov %0, %%ecx" :"=r"(process->registers.ecx));
    asm volatile("mov %0, %%eax" :"=r"(process->registers.eax));
    asm volatile("mov %0, %%edi" :"=r"(process->registers.edi));
    asm volatile("mov %0, %%esi" :"=r"(process->registers.esi));
    // asm volatile("mov %0, %%ebp" :"=r"(process->registers.ebp));
    // asm volatile("mov %0, %%cs" :"=r"(process->registers.cs));
    // asm volatile("mov %0, %%ss" :"=r"(process->registers.ss));
    asm volatile("push %0; popf" :"=r"(process->registers.eflags));

    // response process pages
    kernel::paging::SetDirectory(process->directory);

    return saved_ip;
}

void ProcessScheduler::_ExecuteProcess(KernelProcess* process)
{
#ifdef K_LOG_SCHEDULER
    LOG_SA("SCHEDULER:", "Serving process %d\n", process->pid)
#endif
    _CurrentProcess = process;

    DisableHardwareInterrupts();

    if (process->registers_set == false)
    {
        // we need to set up the registers
        process->registers.esp = (uint32_t)process->stack_begin;
        process->registers.ebp = (uint32_t)process->stack_begin;
        process->registers.ds  = (uint32_t)process->data_begin;
    }

     _ContextSwitch(process);

    EnableHardwareInterrupts();

    // jump to the last execution point 
    // asm volatile("jmp %0\n" ::"r"(saved_ip));
    
    //EnableHardwareInterrupts();
}

void scheduler::run_process_scheduler(RegistersStruct_x86_32* regs, void* args)
{
    // save the last registers
    if (_CurrentProcess)
    {
        _CurrentProcess->registers_set = true;
        memcpy(&_CurrentProcess->registers, regs, sizeof(_CurrentProcess->registers));
    }

    auto scheduler = static_cast<ProcessScheduler*>(args);
    scheduler->Run(nullptr);
}
