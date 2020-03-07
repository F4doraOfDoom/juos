#include <kernel/timer.h>

uint64_t timer::__tick_counter = 0;
struct {
    bool is_asleep = 0;
    uint64_t ticks_left = 0;
} __thread_sleep_info;

void timer::start(uint32_t clock_freq)
{
    interrupts::set_handler(TIMER_PORT, timer::__tick_handler);
    // sometimes the hardware calls this interrupt number for the clock.
    // we ignore it by passing it an empty function
    interrupts::set_handler(TIMER_PORT + 1, [](void*) {} );
 
    // call the arch's intialization of time
    timer::initialize(clock_freq);
}

void timer::sleep(uint32_t slices)
{
    __thread_sleep_info.is_asleep = true;
    __thread_sleep_info.ticks_left = slices;

    while (__thread_sleep_info.is_asleep)
    {
        __asm volatile("hlt;");
    }
}

uint64_t timer::current_time()
{
    return __tick_counter;
}

void timer::__tick_handler(void*)
{
    timer::__tick_counter++;

    if (__thread_sleep_info.is_asleep)
    {
        if (--__thread_sleep_info.ticks_left <= 0)
        {
            __thread_sleep_info.is_asleep = false;
        }
    }
//#ifdef K_LOG
//     if (timer::__tick_counter & 1000 == 0)
//     {
//      LOG_SA("TIMER: ", "Reached tick: %d\n", timer::__tick_counter);
//     }
// //#endif
    //printf("Tick! %d\n", timer::__tick_counter, timer::__tick_counter, timer::__tick_counter, timer::__tick_counter);
}

