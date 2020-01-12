#ifndef KERNEL_NEW_H_
#define KERNEL_NEW_H_

#include "kdef.h"
#include "kmm.h"
#include "klog.h"


NAMESPACE_BEGIN(kernel)

    /**
     * @brief Custome memory allocator, that uses __primitive_heap
     * as an allocation space. Has no ability to deconstruct an object,
     * since you cannot deallocate on __primitive heap 
     * More documentation inside of IAllocator (libstdcxx/allocator.hpp)
     * 
     * @tparam Type - Allocator will alocate this type 
     */
    template <typename Type>
    struct PrimitiveAllocator
    {
        static constexpr uint32_t     object_size = sizeof(Type);     

        typedef Type                value_type;
        typedef Type&               reference;
        typedef const Type&         const_reference;
        typedef Type*               pointer; 

        static pointer allocate(uint32_t n)
        {
            return (pointer)kernel::heap::allocate(n * object_size);
        }

        static void construct(pointer p, const_reference v)
        {
            *p = Type(v);
        }

        static void destroy(pointer p)
        {
            p->~Type();
        }
    };

    /**
     * @brief Custome memory allocator, that uses __mapped_heap
     * as an allocation space. Has the ability to deallocate memory.
     * More documentation inside of IAllocator (libstdcxx/allocator.hpp)
     * 
     * @tparam Type - Allocator will alocate this type 
     */
    template <typename Type>
    struct AdvancedAllocator
    {
        static constexpr uint32_t     object_size = sizeof(Type);     

        typedef Type                value_type;
        typedef Type&               reference;
        typedef const Type&         const_reference;
        typedef Type*               pointer; 

        static pointer allocate(uint32_t n)
        {
            return (pointer)kernel::memory_manager::malloc(n * object_size);
        }

        static void deallocate(pointer p, size_t n)
        {
            kernel::memory_manager::free(p);
        }

        static void construct(pointer p, const_reference v)
        {
            *p = Type(v);
        }

        static void destroy(pointer p)
        {
            p->~Type();
        }
    };

NAMESPACE_END(kernel)

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


#endif // KERNEL_NEW_H_