#include <arch/i386/pit.h>
#include <kernel/klog.h>

void Timer::Initialize(uint32_t clock_freq)
{
    LOG_SA("TIMER: ", "Initializing... Frequency: %d\n", clock_freq);
    Pit::_Init(clock_freq);
}

void Pit::_Init(uint32_t clock_freq)
{
    uint32_t divisor = PIT_FREQ / clock_freq;

    // initiate the PIT to recieve a command
    outb(PIT_COMMAND_PORT, PIT_COMMAND_BYTE);

    outb(PIT_DPORT_1, (uint8_t)(divisor & 0xFF));
    outb(PIT_DPORT_1, (uint8_t)((divisor >> 8) & 0xFF));
}