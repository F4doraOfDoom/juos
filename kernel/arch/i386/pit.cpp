#include <include/pit.h>

void timer::initialize(uint32_t clock_freq)
{
    pit::_init(clock_freq);
}

void pit::_init(uint32_t clock_freq)
{
    uint32_t divisor = PIT_FREQ / clock_freq;

    // initiate the PIT to recieve a command
    outb(PIT_COMMAND_PORT, PIT_COMMAND_BYTE);

    outb(PIT_DPORT_1, (uint8_t)(divisor & 0xFF));
    outb(PIT_DPORT_1, (uint8_t)((divisor >> 8) & 0xFF));
}