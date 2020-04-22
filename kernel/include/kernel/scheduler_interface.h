#ifndef KERNEL_SCHEDUALER_INTERFACE_H
#define KERNEL_SCHEDUALER_INTERFACE_H

#include <kernel/kdef.h>

NAMESPACE_BEGIN(kernel)

    NAMESPACE_BEGIN(scheduler)

        template <class ItemType>
        class IScheduler
        {
        public: 
            /**
             * @brief This function should be called when context switching. 
             * Upon calling it, the scheduler should set it's next process. 
             * 
             * @param regs - registers when context switching
             * @param args - optional arguments
             */
            virtual void CalculateNext(RegistersStruct_x86_32* regs, void* args) = 0;

            /**
             * @brief Get the Next process
             * 
             * @return ItemType* 
             */
            virtual ItemType* GetNext() = 0;

            /**
             * @brief Add a process to the process list
             * 
             * @param item 
             */
            virtual void AddItem(ItemType* item) = 0;

            /**
             * @brief Tell the scheduler that process _pid_ has finished.
             * 
             * @param pid 
             */
            virtual void SignalEnd(uint32_t pid) = 0;
        };

    NAMESPACE_END(scheduler)

NAMESPACE_END(kernel)

#endif // KERNEL_SCHEDUALER_INTERFACE_H