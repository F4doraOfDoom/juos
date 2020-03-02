#ifndef KERNEL_MEM_OPS_H_
#define KERNEL_MEM_OPS_H_

#include <kernel/kmm.h>

void* operator new(size_t size)
{
#ifdef K_LOG_MALLOC
    LOG_SA("NEW: ", "Allocating object the size of %d\n", size);
#endif

    return kernel::memory_manager::malloc(size);
}

void* operator new[](size_t size)
{
#ifdef K_LOG_MALLOC
    LOG_SA("NEW: ", "Allocating object[] the size of %d\n", size);
#endif

    return kernel::memory_manager::malloc(size);
}

void operator delete(void* ptr)
{
#ifdef K_LOG_MALLOC
    LOG_SA("NEW: ", "Deleting object the at %p\n", ptr);
#endif

    kernel::memory_manager::free(ptr);
}

void operator delete(void* ptr, long unsigned int)
{
#ifdef K_LOG_MALLOC
    LOG_SA("NEW: ", "Deleting object the at %p\n", ptr);
#endif

    kernel::memory_manager::free(ptr);
}

void operator delete[](void* ptr, long unsigned int)
{
#ifdef K_LOG_MALLOC
    LOG_SA("NEW: ", "Deleting object[] the at %p\n", ptr);
#endif

    kernel::memory_manager::free(ptr);
}

void operator delete[](void* ptr)
{
#ifdef K_LOG_MALLOC
    LOG_SA("NEW: ", "Deleting object[] the at %p\n", ptr);
#endif

    kernel::memory_manager::free(ptr);
}

#endif // KERNEL_MEM_OPS_H_