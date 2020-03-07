#include <kernel/kprocess.h>

using namespace kernel;

using data_structures::Vector;
using processing::RegisteredProcess;

static Vector<processing::RegisteredProcess>    _registered_processes;
Vector<processing::Process>                        current_tasks;

void processing::start::code(const void* code_ptr)
{
    _set_instruction_ptr((uint32_t*)code_ptr);
}

void processing::register_process(String name, const void* func)
{
    _registered_processes.push_back(RegisteredProcess{name, func});
}
