#include <kernel/kprocess.h>

using namespace kernel;

void processing::run_code(const void* code_ptr)
{
    _set_instruction_ptr((uint32_t*)code_ptr);
}