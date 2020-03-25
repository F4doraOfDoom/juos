#ifndef KERNEL_PIT_I386
#define KERNEL_PIT_I386

#include <stdint.h>

#include <kernel/timer.h>
#include <kernel/kdef.h>
#include <kernel/kcommon.h>

NAMESPACE_BEGIN(Pit)

    #define PIT_DPORT_1         0x40
    #define PIT_DPORT_2         0x41
    #define PIT_DPORT_3         0x42
    #define PIT_COMMAND_PORT    0x43

    #define PIT_FREQ            1193180
    #define PIT_COMMAND_BYTE    0x36

    static void _Init(uint32_t clock_freq);

NAMESPACE_END(Pit)

#endif // KERNEL_PIT_I386