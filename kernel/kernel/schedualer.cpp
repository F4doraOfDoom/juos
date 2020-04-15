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
    while (cur->next)
    {
        cur = cur->next;
    }

    cur->next = new ProcessNode();
    cur->next->proccess = process;
}

KernelProcess* ProcessScheduler::GetNext()
{
    return _current_process->proccess;
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

__ALIGNED(0x1000) paging::PageDirectory _temp_dir;

void scheduler::SwitchTask(void* args)
{
    // If we haven't initialised tasking yet, just return.
    if (!_current_process)
        return;

    // Read esp, ebp now for saving later on.
    uint32_t esp, ebp, eip;
    asm volatile("mov %%esp, %0" : "=r"(esp));
    asm volatile("mov %%ebp, %0" : "=r"(ebp));

    // Read the instruction pointer. We do some cunning logic here:
    // One of two things could have happened when this function exits - 
    //   (a) We called the function and it returned the EIP as requested.
    //   (b) We have just switched tasks, and because the saved EIP is essentially
    //       the instruction after read_eip(), it will seem as if read_eip has just
    //       returned.
    // In the second case we need to return immediately. To detect it we put a dummy
    // value in EAX further down at the end of this function. As C returns values in EAX,
    // it will look like the return value is this dummy value! (0x12345).
    eip = get_ip();

    // Have we just switched tasks?
    if (eip == 0x12345)
        return;

    // No, we didn't switch tasks. Let's save some register values and switch.
    _current_process->proccess->registers.eip = eip;
    _current_process->proccess->registers.esp = esp;
    _current_process->proccess->registers.ebp = ebp;
    
    // Get the next task to run.
    _current_process = _current_process->next;
    // If we fell off the end of the linked list start again at the beginning.
    if (!_current_process) _current_process = _process_list;

    eip = _current_process->proccess->registers.eip;
    esp = _current_process->proccess->registers.esp;
    ebp = _current_process->proccess->registers.ebp;

    memcpy(&_temp_dir, _current_process->proccess->directory, sizeof(paging::PageDirectory));
    // Make sure the memory manager knows we've changed page directory.
    paging_current_directory = &_temp_dir;
    // Here we:
    // * Stop interrupts so we don't get interrupted.
    // * Temporarily puts the new EIP location in ECX.
    // * Loads the stack and base pointers from the new task struct.
    // * Changes page directory to the physical address (physicalAddr) of the new directory.
    // * Puts a dummy value (0x12345) in EAX so that above we can recognise that we've just
    //   switched task.
    // * Restarts interrupts. The STI instruction has a delay - it doesn't take effect until after
    //   the next instruction.
    // * Jumps to the location in ECX (remember we put the new EIP in there).
    asm volatile("         \
      cli;                 \
      mov %0, %%ecx;       \
      mov %1, %%esp;       \
      mov %2, %%ebp;       \
      mov %3, %%cr3;       \
      mov $0x12345, %%eax; \
      sti;                 \
      jmp *%%ecx           "
                 : : "r"(eip), "r"(esp), "r"(ebp), "r"(paging_current_directory->table_addresses));
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
