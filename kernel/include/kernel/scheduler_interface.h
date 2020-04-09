#ifndef KERNEL_SCHEDUALER_INTERFACE_H
#define KERNEL_SCHEDUALER_INTERFACE_H

#include <kernel/kdef.h>

NAMESPACE_BEGIN(kernel)

    NAMESPACE_BEGIN(scheduler)

        template <class ItemType>
        class IScheduler
        {
        public: 
            // actual function for the scheduler to set a _next variable
            virtual void CalculateNext(RegistersStruct_x86_32* regs, void* args) = 0;

            virtual ItemType* GetNext() = 0;

            virtual void AddItem(ItemType* item) = 0;

            // signal for the scheduler that the process has finished
            virtual void SignalEnd(uint32_t pid) = 0;
        };

    NAMESPACE_END(scheduler)

NAMESPACE_END(kernel)

#endif // KERNEL_SCHEDUALER_INTERFACE_H