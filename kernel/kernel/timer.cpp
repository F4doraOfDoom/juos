#include <kernel/timer.h>

void timer::start(uint32_t clock_freq)
{
    interrupts::set_handler(TIMER_PORT, timer::__tick_handler);
    // sometimes the hardware calls this interrupt number for the clock.
    // we ignore it by passing it an empty function
    interrupts::set_handler(TIMER_PORT + 1, [](void*) {} );
 
    // call the arch's intialization of time
    timer::initialize(clock_freq);
}

void timer::__tick_handler(void*)
{
    timer::__tick_counter++;
//#ifdef K_LOG
//     if (timer::__tick_counter & 1000 == 0)
//     {
//      LOG_SA("TIMER: ", "Reached tick: %d\n", timer::__tick_counter);
//     }
// //#endif
    //printf("Tick! %d\n", timer::__tick_counter, timer::__tick_counter, timer::__tick_counter, timer::__tick_counter);
}