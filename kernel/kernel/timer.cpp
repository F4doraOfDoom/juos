#include <kernel/timer.h>
#include <kernel/processing.h>

struct FuncArgPair
{
    Timer::CallableFunc func;
    void* args;
};

static Vector<FuncArgPair> _functions_to_call;
uint64_t Timer::__tick_counter = 0;
struct {
    bool is_asleep = 0;
    uint64_t ticks_left = 0;
} __thread_sleep_info;

void Timer::start(uint32_t clock_freq)
{
    Interrupts::set_handler(TIMER_PORT, Timer::__tick_handler);
    // sometimes the hardware calls this interrupt number for the clock.
    // we ignore it by passing it an empty function
    Interrupts::set_handler(TIMER_PORT + 1, [](void*) {} );
 
    // call the arch's intialization of time
    Timer::Initialize(clock_freq);
}

void Timer::sleep(uint32_t slices)
{
    // __thread_sleep_info.is_asleep = true;
    // __thread_sleep_info.ticks_left = slices;

    // while (__thread_sleep_info.is_asleep)
    // {
    //     __asm volatile("hlt;");
    // }
}

void Timer::add_callable_function(CallableFunc func, void* args)
{
    _functions_to_call.push_back({func, args});
}

uint64_t Timer::current_time()
{
    return __tick_counter;
}

__NO_MANGELING void _SetDirectory(paging::PageDirectory* dir)
{
    paging::SetDirectory(dir);
}

void Timer::__tick_handler(void* reg)
{
    using namespace kernel;
 
    static Processing::Context _current_context;
    static uint32_t next_eip, next_times_ran;
    // static Processing::Context _next_context;
    // static Processing::KernelProcess::ProcessResolver _next_on_end;
    // static uint32_t _next_id = 0;
    // static uint32_t _next_run_times = 0;

    DisableHardwareInterrupts();

    // restore kernel address space
    paging::SetDirectory(paging::GetKernelDirectory());
 
    auto current_process = scheduler::GetCurrentProcess();

    // handle timer callbacks
    // here the scheduler will set the next process to run,
    // so we need to save the current process
    for (const auto func_args : _functions_to_call)
    {
        func_args.func(static_cast<RegistersStruct_x86_32*>(reg), func_args.args);
    }
    
    // staring schedualing next process
    auto next_process = Processing::GetScheduler()->GetNext();

    // if there is no next process or current process doesn't chance
    if (next_process == nullptr) 
    {
        // no reason to switch processes
        paging::SetDirectory(current_process->directory);
        return;
    }

    // no need to save context if a process isn't running
    if (current_process != nullptr)
    {
        asm volatile("mov %%esp, %0": "=r"(_current_context.esp));
        asm volatile("mov %%ebp, %0": "=r"(_current_context.ebp));

        // non-standard gcc feature to take address of label 
        _current_context.eip = (uint32_t)&&_current_line;
        current_process->ApplyContext(&_current_context);
    }

    // _next_context.eip = next_process->registers.eip;
    // _next_context.esp = next_process->registers.esp;
    // _next_context.ebp = next_process->registers.ebp;
    // _next_on_end    = next_process->on_end;
    // _next_id        = next_process->pid;
    // _next_run_times = next_process->times_ran;
    
    next_eip        = next_process->registers.eip;
    next_times_ran  = next_process->times_ran++; 

    asm volatile("         \
        cli;                 \
        mov %0, %%esp;       \
        mov %1, %%ebp;       \
        mov %2, %%cr3;       "
                 : :  
                    "r"(next_process->registers.esp),
                    "r"(next_process->registers.ebp),
                    "r"(next_process->directory->table_addresses));

    // first time running
    if (next_times_ran == 1)
    {
        asm volatile(
            "mov %0, %%ecx;"
            "sti;" // notice the sti here
            "jmp %%ecx"  
                    : :
                        "b"(next_eip)
        );
    }
    else
    {
        asm volatile(
            "mov %0, %%ecx;"
            "jmp %%ecx"  
                    : :
                        "b"(next_eip)
        );
    }
    
_current_line:
    return;
}

