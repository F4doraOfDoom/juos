#include <kernel/hardware.h>

__NO_MANGELING void EnableHardwareInterrupts()
{
    asm volatile("sti");
}

__NO_MANGELING void DisableHardwareInterrupts()
{
    asm volatile("cli");
}