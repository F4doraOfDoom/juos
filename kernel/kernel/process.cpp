#include <kernel/processing.h>
#include <kernel/timer.h>

using namespace kernel;

using Processing::RegisteredProcess;
using Processing::KernelProcess;
using Processing::ProcessScheduler;
using Processing::CachedProcessInfo;

CachedProcessInfo _current_process_cached_info;


extern paging::PageDirectory* __kernel_directory;
static Vector<RegisteredProcess>        _registered_processes;
static Vector<KernelProcess*>                 _current_processes; 

static ProcessScheduler _scheduler = nullptr;

extern uint32_t __stack_top; // variable indicating the start of the stack

// the main kernel process
KernelProcess* _kernel_process = nullptr;
uint32_t KernelProcess::_pid_seq = 1;

KernelProcess::KernelProcess(const void* func_ptr, Priority priority)
{
    this->priority = priority;

    // the time each process gets to run is equal to
    // its priority on a scale of 1-5, times 10
    slice_size = ((uint64_t)priority + 1) * 10;
    _is_finished = false;
}

KernelProcess::~KernelProcess()
{
    // if it has special mappings
    if (self_mappings)
    {
        delete self_mappings;
    }

    // if it has an input buffer 
    if (input_buffer)
    {
        delete input_buffer;
    }

    // we don't want to destroy the kernel's directory
    if (pid > 1)
    {
        delete directory;
    }
}

void KernelProcess::ApplyContext(const Context* context)
{
    registers.eip = context->eip;
    registers.esp = context->esp;
    registers.ebp = context->ebp;
}

static void _SignalProcEnd(KernelProcess::ID proc_id, void* args)
{
    //DISABLE_HARDWARE_INTERRUPTS();
    Processing::GetScheduler()->SignalEnd(proc_id);
    //ENABLE_HARDWARE_INTERRUPTS();
    // while (true)
    // {
       //  asm volatile("hlt");
    // }
};

static KernelProcess* _NewProcess(const void* start, KernelProcess::Priority priority, Vector<paging::_HeapMappingSettings>* mappings)
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
    new_process->registers.esp = (uint32_t)new_process->stack_begin + 0x1000;
    new_process->registers.ebp = (uint32_t)new_process->stack_begin + 0x1000;

    new_process->on_end = _SignalProcEnd;

    new_process->self_mappings = mappings;

    return new_process;
}

ProcessScheduler Processing::GetScheduler()
{
    return _scheduler;
}

void Processing::Initialize(KernelStart start, SchedulerCallback callback, ProcessScheduler scheduler)
{
    _scheduler = scheduler;

    // we pass no mappings because we're going to use the kernel's page directory
    _kernel_process = _NewProcess((void*)start, KernelProcess::Priority::System, nullptr);

    // we want to use the kernel's heap
    delete _kernel_process->directory;
    auto kernel_directory = paging::GetKernelDirectory();
    _kernel_process->directory = kernel_directory;

    paging::map_region(KERNEL_STACK_BEGIN, KERNEL_STACK_BEGIN + KERNEL_STACK_SIZE, paging::StandartAllocator, kernel_directory);

    // copy the old stack to the new stack
    DISABLE_HARDWARE_INTERRUPTS();

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

    ENABLE_HARDWARE_INTERRUPTS();

    _scheduler->AddItem(_kernel_process);
    Timer::add_callable_function(callback, scheduler);
}

KernelProcess::ID Processing::GetPid()
{
    return _current_process_cached_info.pid;
}

void Processing::Start::Code(const void* code_ptr)
{
    _set_instruction_ptr((uint32_t*)code_ptr);
}

KernelProcess::ID Processing::Start::Process(const String& name, Processing::KernelProcess::Priority priority)
{
    //DISABLE_HARDWARE_INTERRUPTS();

    static uint32_t             _current_directory;
    static KernelProcess::ID    _new_proc_id;
    static char                 _proc_name_buffer[128];

    _current_directory = 0;
    memset(_proc_name_buffer, 0, sizeof(_proc_name_buffer));
    memcpy(_proc_name_buffer, name.c_str(), sizeof(_proc_name_buffer));

    // if the process is not the kernel, we're going to need to switch to the kernel
    if (_current_process_cached_info.pid != 0)
    {
        asm volatile("mov %%cr3, %0" : "=r"(_current_directory));
        asm volatile("mov %0, %%cr3" :: "r"(__kernel_directory->table_addresses));
    }

    auto proc = std::find_if(_registered_processes.begin(), _registered_processes.end(), [&](const RegisteredProcess& rp)
    {
        return rp.name.compare(_proc_name_buffer);
    });
 
    
    auto proc_mappings = new Vector<paging::_HeapMappingSettings>();
    proc_mappings->push_back({0xA0000000, 0xA0010000}); // stack
    proc_mappings->push_back({K_HEAP_START, K_HEAP_START + K_HEAP_INITIAL_SIZE}); // heap
    //proc_mappings.push_back({0xD0000000, 0xD0010000}); // data

    auto new_process = _NewProcess(proc->func_ptr, priority, proc_mappings);

    _new_proc_id = new_process->pid;

    _scheduler->AddItem(new_process);

    

    // now return the current directory
    if (_current_process_cached_info.pid != 0)
    {
        asm volatile("mov %0, %%cr3" :: "r"(_current_directory));
    }

    //ENABLE_HARDWARE_INTERRUPTS();

    return _new_proc_id;
}

Queue<keyboard::InputKeyType>* Processing::GetInputBuffer()
{
    return *_current_process_cached_info.input_buffer_ptr;
}

void Processing::LockInputBuffer()
{
    LOCK(*_current_process_cached_info.input_buffer_lock_ptr)
}

void Processing::UnlockInputBuffer()
{
    UNLOCK(*_current_process_cached_info.input_buffer_lock_ptr)
}


void Processing::End::Process(KernelProcess::ID pid)
{
    static uint32_t temp_dir = 0, temp_pid = 0;
    temp_dir = 0;

    temp_pid = pid;

    // we need to switch into kernel context
    if (_current_process_cached_info.pid != 1)
    {
        asm volatile ("mov %%cr3, %0" : "=r"(temp_dir));
        asm volatile ("mov %0, %%cr3" :: "r"(__kernel_directory->table_addresses));
    }

    _scheduler->SignalEnd(temp_pid);

    // we need to switch into kernel context
    if (_current_process_cached_info.pid != 1)
    {
        asm volatile ("mov %0, %%cr3" :: "r"(temp_dir));
    }
}

void Processing::SelfProcessInit()
{
    *_current_process_cached_info.input_buffer_ptr = new Queue<keyboard::InputKeyType>();
}

void Processing::RegisterProcess(const String& name, const void* func)
{
    _registered_processes.push_back(RegisteredProcess{name, func});
}


