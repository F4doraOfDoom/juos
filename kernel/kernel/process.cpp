#include <kernel/processing.h>
#include <kernel/timer.h>

using namespace kernel;

using Processing::RegisteredProcess;
using Processing::KernelProcess;
using Processing::ProcessScheduler;

static Vector<RegisteredProcess>        _registered_processes;
static Vector<KernelProcess*>                 _current_processes; 

static ProcessScheduler _scheduler = nullptr;

extern uint32_t __stack_top; // variable indicating the start of the stack

// the main kernel process
KernelProcess* _kernel_process = nullptr;
uint64_t KernelProcess::_pid_seq = 0;

KernelProcess::KernelProcess(const void* func_ptr, Priority priority)
{
    active_threads.push_back(Thread{func_ptr});
    this->priority = priority;

    // the time each process gets to run is equal to
    // its priority on a scale of 1-5, times 10
    slice_size = ((uint64_t)priority + 1) * 10;
    _is_finished = false;
}

static KernelProcess* NewProcess(const void* start, KernelProcess::Priority priority, Vector<paging::_HeapMappingSettings>* mappings)
{
    auto new_process = new KernelProcess(start, priority);

    // mappings == null -> create unmapped process
    if (mappings)
    {
        new_process->directory = paging::create_directory(*mappings);
    }

    new_process->stack_begin = (void*)KERNEL_STACK_BEGIN;
    new_process->heap_begin = (void*)K_HEAP_START;
    //new_process->data_begin = (void*)0xD0000000;

    new_process->registers.eip = (uint32_t)start;
    new_process->registers.esp = (uint32_t)new_process->stack_begin;
    new_process->registers.ebp = (uint32_t)new_process->stack_begin;

    return new_process;
}

void Processing::Initialize(KernelStart start, SchedulerCallback callback, ProcessScheduler scheduler)
{
    _scheduler = scheduler;

    // we pass no mappings because we're going to use the kernel's page directory
    _kernel_process = NewProcess((void*)start, KernelProcess::Priority::System, nullptr);

    // we want to use the kernel's heap
    delete _kernel_process->directory;
    auto kernel_directory = paging::GetKernelDirectory();
    _kernel_process->directory = kernel_directory;

    paging::map_region(KERNEL_STACK_BEGIN, KERNEL_STACK_BEGIN + KERNEL_STACK_SIZE, paging::StandartAllocator, kernel_directory);

    // copy the old stack to the new stack
    DisableHardwareInterrupts();

    paging::SetDirectory(kernel_directory);

    // stack will be an offset from zero
    uint32_t esp = 0, ebp = 0;
    asm volatile("mov %%ebp, %0" : "=r"(ebp));
    asm volatile("mov %%esp, %0" : "=r"(esp));

    uint32_t stack_offset = KERNEL_STACK_BEGIN - __stack_top + 0x1000;
    uint32_t new_esp = esp + stack_offset;
    uint32_t new_ebp = ebp + stack_offset;

    memcpy((void*)new_esp, (void*)esp, __stack_top - esp);

    asm volatile("mov %0, %%esp" :: "r"(new_esp));
    asm volatile("mov %0, %%ebp" :: "r"(new_ebp));

    EnableHardwareInterrupts();

    _scheduler->AddItem(_kernel_process);
    Timer::add_callable_function(callback, scheduler);
}

void Processing::Start::Code(const void* code_ptr)
{
    _set_instruction_ptr((uint32_t*)code_ptr);
}

const KernelProcess* Processing::Start::Process(const String& name, Processing::KernelProcess::Priority priority)
{
    auto proc = std::find_if(_registered_processes.begin(), _registered_processes.end(), [&](const RegisteredProcess& rp)
    {
        return rp.name.compare(name);
    });
 
    Vector<paging::_HeapMappingSettings> proc_mappings;
    proc_mappings.push_back({0xA0000000, 0xA0010000}); // stack
    proc_mappings.push_back({K_HEAP_START, K_HEAP_START + K_HEAP_INITIAL_SIZE}); // heap
    //proc_mappings.push_back({0xD0000000, 0xD0010000}); // data

    auto new_process = NewProcess(proc->func_ptr, priority, &proc_mappings);
    
    _scheduler->AddItem(new_process);

    return new_process;
}

void Processing::RegisterProcess(const String& name, const void* func)
{
    _registered_processes.push_back(RegisteredProcess{name, func});
}


