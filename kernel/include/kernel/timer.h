#ifndef KERNEL_TIMER_H_
#define KERNEL_TIMER_H_

#include <stdint.h>
#include "kdef.h"


NAMESPACE_BEGIN(timer)

    enum class TimerDevice 
    {
        Pit // Programmable Interval Timer
    };

    void initialize(uint32_t clock_freq);

NAMESPACE_END(timer)

#endif //KERNEL_TIMER_H_