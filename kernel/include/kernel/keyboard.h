#ifndef KERNEL_KEYBOARD_INTERFACE_H
#define KERNEL_KEYBOARD_INTERFACE_H

#include <kernel/interrupts.h>
#include <kernel/kdef.h>

#include <stdint.h>

NAMESPACE_BEGIN(kernel)

    NAMESPACE_BEGIN(keyboard)

        struct InputKeyType
        {
            uint32_t character;
        };

        using KeyboardCallback = InputKeyType (*)(void*);

        void InitializeKeyboard(KeyboardCallback source);

        void KeyboardHandler(void*);

    NAMESPACE_END(keyboard)

NAMESPACE_END(kernel)

#endif // KERNEL_SCHEDUALER_INTERFACE_H