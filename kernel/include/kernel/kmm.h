/**
 * @file kmm.h
 * @author Jonathan Uklisty (F4doraOfDoom) (yoniu7@gmail.com)
 * @brief Kernel memory managment - malloc and free
 * @version 0.1
 * @date 12-11-2019
 * 
 * @copyright Copyright Jonathan Uklisty (c) 2019 (MIT License)
 * 
 */

#include <stdint.h>
#include <allocator.hpp>

#include "kdef.h"
#include "kheap.h"

#define KHEAP_START         0xC0000000
#define KHEAP_INITIAL_SIZE  0x100000
#define KHEAP_INDEX_SIZE    0x20000
#define KHEAP_MAGIC         0x12345678
#define KHEAP_MIN_SIZE      0x70000

//TODO: Implement better heap after I get this to work

NAMESPACE_BEGIN(kernel)

    NAMESPACE_BEGIN(memory)

        /**
         * @brief Documentation inside of IAllocator (libstdcxx/allocator.hpp)
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

            pointer allocate(uint32_t n)
            {
                return (pointer)kernel::heap::allocate(n * object_size);
            }

            void construct(pointer p, const_reference v)
            {
                *p = Type(v);
            }

            void destroy(pointer p)
            {
                p->~Type();
            }

        };

        struct ChunkHeader
        {
            uint32_t    magic;
            uint8_t     is_hole;
            uint32_t    size;
        };

        struct Chunk
        {
            uint32_t        magic;
            ChunkHeader*    header;
        };

    NAMESPACE_END(memory)

NAMESPACE_END(kernel)