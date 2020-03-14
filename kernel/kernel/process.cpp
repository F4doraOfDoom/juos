#include <kernel/kprocess.h>

using namespace kernel;

using processing::RegisteredProcess;
using processing::Process;

static Vector<RegisteredProcess>        _registered_processes;
static Vector<Process*>                 _current_processes; 

uint64_t Process::_pid_seq = 0;

void processing::start::code(const void* code_ptr)
{
    _set_instruction_ptr((uint32_t*)code_ptr);
}

Process* processing::start::process(const String& name, processing::Process::Priority priority)
{
    auto proc = std::find_if(_registered_processes.begin(), _registered_processes.end(), [&](const RegisteredProcess& rp)
    {
        return rp.name.compare(name);
    });

    auto new_process = new Process(proc->func_ptr, priority);

    _current_processes.push_back(new_process);

    return new_process;
}

void processing::register_process(const String& name, const void* func)
{
    _registered_processes.push_back(RegisteredProcess{name, func});
}


