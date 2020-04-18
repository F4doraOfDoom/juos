#ifndef KERNEL_DRIVERS_PS2_H
#define KERNEL_DRIVERS_PS2_H

#include <kernel/kdef.h>
#include <kernel/kcommon.h>
#include <kernel/keyboard.h>

#include <stdint.h>

NAMESPACE_BEGIN(drivers)

    NAMESPACE_BEGIN(ps2)

        constexpr inline uint32_t DATA_PORT = 0x60;
        
        NAMESPACE_BEGIN(keyboard)

            kernel::keyboard::InputKeyType KeyboardSource(void* args);

        NAMESPACE_END(keyboard)

    NAMESPACE_END(ps2)

NAMESPACE_END(drivers)

#endif // KERNEL_DRIVERS_PS2_H