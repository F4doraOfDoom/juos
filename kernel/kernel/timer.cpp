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

static Processing::Context _current_context;
void Timer::__tick_handler(void* reg)
{
    using namespace kernel;
    // TODO: Reimplement sleeping for multiprocessing
    // if (__thread_sleep_info.is_asleep)
    // {
    //     if (--__thread_sleep_info.ticks_left <= 0)
    //     {
    //         __thread_sleep_info.is_asleep = false;
    //     }
    // }

    Processing::GetCurrentContext(&_current_context);

    // restore kernel address space
    paging::SetDirectory(paging::GetKernelDirectory());

    // handle timer callbacks
    for (const auto func_args : _functions_to_call)
    {
        func_args.func(static_cast<RegistersStruct_x86_32*>(reg), func_args.args);
    }

//#ifdef K_LOG
//     if (timer::__tick_counter & 1000 == 0)
//     {
//      LOG_SA("TIMER: ", "Reached tick: %d\n", timer::__tick_counter);
//     }
// //#endif
    //printf("Tick! %d\n", timer::__tick_counter, timer::__tick_counter, timer::__tick_counter, timer::__tick_counter);
}

