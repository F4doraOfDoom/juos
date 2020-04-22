#ifndef KERNEL_NEW_H_
#define KERNEL_NEW_H_

#include "kdef.h"
#include "kuseful.h"
#include "klog.h"

#include <kernel/mem_operators.hpp>

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
            return (pointer)kernel::heap::Allocate(n * object_size);
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

        /**
         * @brief Allocate _n_ objects of size Type
         * 
         * @param n 
         * @return pointer 
         */
        static pointer allocate(uint32_t n)
        {
            return new Type[n];
        }

        /**
         * @brief Deallocate a pointer pointing to _n_ objects of type Type
         * 
         * @param p 
         * @param n 
         */
        static void deallocate(pointer p, size_t n)
        {
            delete p;
        }

        /**
         * @brief construct an object of type Type in pointer _p_ with args _v_
         * 
         * @param p 
         * @param v 
         */
        static void construct(pointer p, const_reference v)
        {
            *p = Type(v);
        }

        /**
         * @brief Destroy an object of type Type in pointer _p_
         * 
         * @param p 
         */
        static void destroy(pointer p)
        {
            p->~Type();
        }
    };

NAMESPACE_END(kernel)



#endif // KERNEL_NEW_H_