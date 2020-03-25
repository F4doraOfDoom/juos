#include <kernel/processing.h>

using namespace kernel;

using Processing::RegisteredProcess;
using Processing::KernelProcess;
using Processing::ProcessScheduler;

static Vector<RegisteredProcess>        _registered_processes;
static Vector<KernelProcess*>                 _current_processes; 

static ProcessScheduler _scheduler = nullptr;

uint64_t KernelProcess::_pid_seq = 0;

void Processing::Initialize(ProcessScheduler scheduler)
{
    _scheduler = scheduler;
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

    auto new_process = new KernelProcess(proc->func_ptr, priority);

    paging::_HeapMappingSettings proc_mappings {0xA0000000, 0xA0100000};
    new_process->directory = paging::create_directory(&proc_mappings);

    _scheduler->AddItem(new_process);

    return new_process;
}

void Processing::RegisterProcess(const String& name, const void* func)
{
    _registered_processes.push_back(RegisteredProcess{name, func});
}


