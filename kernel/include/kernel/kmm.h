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
#include <string.h>
#include <allocator.hpp>

#include "klog.h"
#include "kdef.h"
#include "kheap.h"
#include "paging.h"

#define K_HEAP_START         0xC0000000
#define K_HEAP_INITIAL_SIZE  0x100000
#define K_HEAP_INDEX_SIZE    0x20000
#define K_HEAP_MAGIC         0x12345678
#define K_HEAP_MIN_SIZE      0x70000

//TODO: Implement better heap after I get this to work

NAMESPACE_BEGIN(kernel)

    NAMESPACE_BEGIN(memory_manager)

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
            bool is_present;
        };

        struct Chunk
        {
            ChunkHeader     header;
            Chunk*          prev;
            uint32_t        size;
        };

        struct Heap
        {
            Chunk*          head;
            uint32_t        start_address; // The start of our allocated space.
            uint32_t        end_address;   // The end of our allocated space. May be expanded up to max_address.
            uint32_t        max_address;   // The maximum address the heap can be expanded to.
            bool            is_kernel;     // Should extra pages requested by us be mapped as supervisor-only?
            bool            rw;       // Should extra pages requested by us be mapped as read-only?
        };

        typedef ChunkHeader chunk_header_t;
        typedef Chunk       chunk_t;
        typedef Heap        heap_t;

        void initialize(uint32_t start, uint32_t end, uint32_t size, bool is_kernel, bool rw);

        void* malloc(uint32_t size);

        void free(void* p);

    NAMESPACE_END(memory_manager)

NAMESPACE_END(kernel)