#include <kernel/kprocess.h>
#include <list.hpp>
#include <string.h>

using namespace kernel;

static data_structures::Vector<processing::RegisteredProcess>    _registered_processes;
std::list<processing::Task>                                     current_tasks;

void processing::start::code(const void* code_ptr)
{
    _set_instruction_ptr((uint32_t*)code_ptr);
}

void register_process(const char* name, const void* func)
{
    processing::RegisteredProcess p;
    p.func_ptr = func;
    memcpy(p.name, name, processing::RegisteredProcess::MaxNameLength);

    //_registered_processes.push_back(p);
}
