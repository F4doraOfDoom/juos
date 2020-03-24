#ifndef KERNEL_SCHEDUALER_INTERFACE_H
#define KERNEL_SCHEDUALER_INTERFACE_H

#include <kernel/kdef.h>

NAMESPACE_BEGIN(kernel)

    NAMESPACE_BEGIN(scheduler)

        template <class ItemType>
        class IScheduler
        {
        public:
            virtual void Run(void* args) = 0;

            virtual void AddItem(ItemType* item) = 0;
        };

    NAMESPACE_END(scheduler)

NAMESPACE_END(kernel)

#endif // KERNEL_SCHEDUALER_INTERFACE_H