#include <kernel/kprocess.h>

using namespace kernel;

using processing::RegisteredProcess;
using processing::Process;
using processing::ProcessScheduler;

static Vector<RegisteredProcess>        _registered_processes;
static Vector<Process*>                 _current_processes; 

static ProcessScheduler _scheduler = nullptr;

uint64_t Process::_pid_seq = 0;

void processing::initialize(ProcessScheduler scheduler)
{
    _scheduler = scheduler;
}

void processing::start::code(const void* code_ptr)
{
    _set_instruction_ptr((uint32_t*)code_ptr);
}

const Process* processing::start::process(const String& name, processing::Process::Priority priority)
{
    auto proc = std::find_if(_registered_processes.begin(), _registered_processes.end(), [&](const RegisteredProcess& rp)
    {
        return rp.name.compare(name);
    });

    auto new_process = new Process(proc->func_ptr, priority);

    paging::_HeapMappingSettings proc_mappings {0xA0000000, 0xA0100000};
    new_process->directory = paging::create_directory(&proc_mappings);

    _scheduler->AddItem(new_process);

    return new_process;
}

void processing::register_process(const String& name, const void* func)
{
    _registered_processes.push_back(RegisteredProcess{name, func});
}


