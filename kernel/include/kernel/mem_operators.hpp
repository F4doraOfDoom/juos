/**
 * @file mem_operators.hpp
 * @author Jonathan Uklisty
 * @brief This file defined operators new and delete for c++. 
 * The operators use kmm's malloc and free
 * @version 0.1
 * @date 2020-04-22
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef KERNEL_MEM_OPS_H_
#define KERNEL_MEM_OPS_H_

#include <kernel/kmm.h>

void* operator new(size_t size)
{
#ifdef K_LOG_MALLOC
    LOG_SA("NEW: ", "Allocating object the size of %d\n", size);
#endif

    return kernel::MemoryManager::malloc(size);
}

void* operator new[](size_t size)
{
#ifdef K_LOG_MALLOC
    LOG_SA("NEW: ", "Allocating object[] the size of %d\n", size);
#endif

    return kernel::MemoryManager::malloc(size);
}

void operator delete(void* ptr)
{
#ifdef K_LOG_MALLOC
    LOG_SA("NEW: ", "Deleting object the at %p\n", ptr);
#endif

    kernel::MemoryManager::free(ptr);
}

void operator delete(void* ptr, long unsigned int)
{
#ifdef K_LOG_MALLOC
    LOG_SA("NEW: ", "Deleting object the at %p\n", ptr);
#endif

    kernel::MemoryManager::free(ptr);
}

void operator delete[](void* ptr, long unsigned int)
{
#ifdef K_LOG_MALLOC
    LOG_SA("NEW: ", "Deleting object[] the at %p\n", ptr);
#endif

    kernel::MemoryManager::free(ptr);
}

void operator delete[](void* ptr)
{
#ifdef K_LOG_MALLOC
    LOG_SA("NEW: ", "Deleting object[] the at %p\n", ptr);
#endif

    kernel::MemoryManager::free(ptr);
}

#endif // KERNEL_MEM_OPS_H_