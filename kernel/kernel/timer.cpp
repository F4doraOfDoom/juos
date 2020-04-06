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

void Timer::__tick_handler(void* reg)
{
    using namespace kernel;

    static Processing::Context _current_context;
    static Processing::Context _next_context;
    static Processing::KernelProcess::ProcessResolver _next_on_end;
    static uint32_t _next_id = 0;
    static uint32_t _next_run_times = 0;

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

    DisableHardwareInterrupts();

    if (next_process == nullptr)
    {
        // no reason to switch processes
        paging::SetDirectory(current_process->directory);
        return;
    }

    // no need to save context if a process isn't running
    if (current_process != nullptr)
    {
        Processing::GetCurrentContext(&_current_context);
        _current_context.eip = (uint32_t)__builtin_return_address(0);

        current_process->ApplyContext(&_current_context);
    }

    _next_context.eip = next_process->registers.eip;
    _next_context.esp = next_process->registers.esp;
    _next_context.ebp = next_process->registers.ebp;
    _next_on_end    = next_process->on_end;
    _next_id        = next_process->pid;
    _next_run_times = next_process->times_ran;

    paging::SetDirectory(next_process->directory);


    asm volatile("mov %0, %%esp" :: "r"(_next_context.esp));
    asm volatile("mov %0, %%ebp" :: "r"(_next_context.ebp));

    if (_next_run_times == 1)
    {
        // the process has never run yet
        // lets build its stack to include on_end
        asm volatile("push %0; push %1; push %2" :: "r"(_next_id), "r"(0), "r"(_next_on_end));

    }
    asm volatile("mov %0, %%ecx; jmp %%ecx" :: "r"(_next_context.eip));

    EnableHardwareInterrupts();
}

