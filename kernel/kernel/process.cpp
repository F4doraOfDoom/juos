#include <kernel/kprocess.h>

using namespace kernel;

using processing::RegisteredProcess;
using processing::Process;

static Vector<RegisteredProcess>        _registered_processes;
Vector<Process>                         current_processes;

uint64_t Process::_pid_seq = 0;

void processing::start::code(const void* code_ptr)
{
    _set_instruction_ptr((uint32_t*)code_ptr);
}

Process processing::start::process(const String& name, processing::Process::Priority priority)
{
    auto proc_addr = std::find_if(_registered_processes.begin(), _registered_processes.end(), [&](const RegisteredProcess& p)
    {
        return p.name.compare(name);
    })->func_ptr;

    Process process(proc_addr, priority);

    return process;
}

void processing::register_process(String name, const void* func)
{
    _registered_processes.push_back(RegisteredProcess{name, func});
}


