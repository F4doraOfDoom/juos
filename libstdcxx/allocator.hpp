#ifndef _CXX_ALLOCATOR_H
#define _CXX_ALLOCATOR_H

#include "libcxx_def.hpp"

NAMESPACE_BEGIN(std)

    template <class T>
    struct allocator
    {
        static constexpr size_t object_size = sizeof(T);

        typedef T               value_type;
        typedef T*              pointer;
        typedef const T*        const_pointer;
        typedef T&              reference;
        typedef const T&        const_reference;
        typedef size_t          size_type;
        typedef size_t          different_type; 

        template <typename U>
        struct rebind
        {
            typedef allocator<U> other;
        };

        /**
         * @brief Allocate _n_ objects of type T without constructing them
         * 
         * @param n - number of objects to allocate
         * @return pointer - pointer to the first object allocated
         */
        static pointer allocate(size_t n)
        {
            return new T[n];
        }

                /**
         * @brief Allocate an object of type T without constructing them
         * 
         * @return pointer - pointer to the object allocated
         */
        static pointer allocate_single()
        {
            return new T();
        }

        /**
         * @brief Deallocated _n_ objects of Type T without calling their destructors.
         * 
         * @param p - pointer to the first object
         * @param n - number of objects to deallocate
         */
        static void deallocate(pointer p, size_t n)
        {
            return delete(p);
        }

        /**
         * @brief Construct an object of type T, instantiate it with _val_
         * 
         * @param p - pointer to object
         * @param val - value to instantiate
         */
        static void construct(pointer p, const_reference val)
        {
            *p = T(val);
        }

        /**
         * @brief Destroy an object of type T
         * 
         * @param p - pointer to the object
         */
        static void destroy(pointer p)
        {
            p->~T();
        }

        // /**
        //  * @brief Prevents this class from being instantiated
        //  * 
        //  */
        // virtual void __no_impl() = 0;
    };

NAMESPACE_END(std)

#endif // _CXX_ALLOCATOR_H