#ifndef KERNEL_KEYBOARD_INTERFACE_H
#define KERNEL_KEYBOARD_INTERFACE_H

#include <kernel/interrupts.h>
#include <kernel/kdef.h>

#include <stdint.h>

NAMESPACE_BEGIN(kernel)

    NAMESPACE_BEGIN(keyboard)

        struct InputKeyType
        {
            // std::queue is mad becuase it expects the type to be pointerish
            // we create a constructor that takes a pointer to fix that
            InputKeyType(void*) {}
            InputKeyType() {}

            uint32_t character;
            bool error = false;
        };

        using KeyboardCallback = InputKeyType (*)(void*);

        /**
         * @brief Initialize the keyboard
         * 
         * @param source 
         */
        void InitializeKeyboard(KeyboardCallback source);

        /**
         * @brief Keyboard interrupt handler
         * 
         */
        void KeyboardHandler(void*);

    NAMESPACE_END(keyboard)

NAMESPACE_END(kernel)

#endif // KERNEL_SCHEDUALER_INTERFACE_H