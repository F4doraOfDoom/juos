#include <kernel/schedualer.h>
#include <kernel/processing.h>

using namespace kernel::scheduler;

volatile ProcessNode*     _process_list = nullptr;
volatile ProcessNode*     _current_process = nullptr;

extern bool __initiated_processing;

// static void RestoreRegisters(RegistersStruct_x86_32* registers)
// {
//     asm ("push %0\n"
//          "popa" :: "r"(*registers));
// }

KernelProcess* scheduler::GetCurrentProcess()
{
    return nullptr;
}

void ProcessScheduler::AddItem(KernelProcess* process) 
{
    auto cur = _process_list;

    // no processes
    if (cur == nullptr)
    {
        _process_list = new ProcessNode();
        _process_list->proccess = process;
        _current_process = _process_list;
        return;
    }

    // traverse to last
    while (cur)
    {
        cur = cur->next;
    }

    cur->next = new ProcessNode();
    cur->proccess = process;
}

KernelProcess* ProcessScheduler::GetNext()
{
    return nullptr;
}

void ProcessScheduler::CalculateNext(RegistersStruct_x86_32* regs, void* args)
{
    
}

void ProcessScheduler::SignalEnd(uint32_t pid)
{
    // implement some end
#ifdef K_LOG_SCHEDULER
    LOG_SA("SCHEDULER: ", "Finished running process %d\n", pid);
#endif

    _canceled_processes.push_back(pid);
}


void scheduler::SwitchTask(void* args)
{
    //SET_DIRECTORY(paging::GetKernelDirectory());

    // wasn't initiated yet
    //if (_current_process == nullptr) return;

    uint32_t esp, ebp, eip;
    asm volatile("mov %%esp, %0" : "=r"(esp));
    asm volatile("mov %%ebp, %0" : "=r"(ebp));

    eip = get_ip();

    if (eip == 0x12345) return;

    _current_process->proccess->registers.esp = esp; 
    _current_process->proccess->registers.ebp = ebp;
    _current_process->proccess->registers.eip = eip;
      

    _current_process = _current_process->next;
    if (_current_process == nullptr)
    {
        _current_process = _process_list;
    }

    eip = _current_process->proccess->registers.eip;
    esp = _current_process->proccess->registers.esp;
    ebp = _current_process->proccess->registers.ebp;

    auto current_directory = _current_process->proccess->directory; 

    asm volatile("         \
      cli;                 \
      mov %0, %%ecx;       \
      mov %1, %%esp;       \
      mov %2, %%ebp;       \
      mov %3, %%cr3;       \
      mov $0x12345, %%eax; \
      sti;                 \
      jmp *%%ecx           "
                 : : "r"(eip), "r"(esp), "r"(ebp), "r"(current_directory->table_addresses));}

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
