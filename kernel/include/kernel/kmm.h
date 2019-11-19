#ifndef KERNEL_MEMORY_MANAGER_H
#define KERNEL_MEMORY_MANAGER_H

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
#include <static_stack.hpp>

#include "klog.h"
#include "kdef.h"
#include "kheap.h"
#include "paging.h"
#include "kbitset.h"
#include "kuseful.h"


#define MM_LOG_ALLOCATIONS          1
#define MM_LOG_DUMPS                2

#define K_HEAP_START         0xC0000000
#define K_HEAP_INITIAL_SIZE  0x100000
#define K_HEAP_INDEX_SIZE    0x20000
#define K_HEAP_MIN_SIZE      0x70000

// Malloc requests below this size will be fast bins
#define FASTBIN_THRESHOLD       0x40
// Number of fast bins available 
#define FASTBIN_MAX_SIZE        0x64

#define IS_FASTBIN_BIT          1 << 0

//TODO: Implement better heap after I get this to work

NAMESPACE_BEGIN(kernel)

    NAMESPACE_BEGIN(memory_manager)

        struct FastChunk
        {
            uint32_t* ptr_to_heap;
        } __PACKED;

        struct FastBin
        {
            FastChunk*  chunks;
            uint32_t    chunk_size;
            uint32_t    bin_size; 
            uint32_t    start_addr;
            uint32_t    end_addr;
        } __PACKED;

        struct BigChunk
        {
            BigChunk*       prev;
            BigChunk*       next;
            uint32_t        size;
            bool            used;
        } __PACKED;

        struct Heap
        {
            uint32_t*       fast_bins;
            BigChunk*       big_chunks;
            uint32_t        allocated_chunks;
            uint32_t        start_address; // The start of our allocated space.
            uint32_t        end_address;   // The end of our allocated space. May be expanded up to max_address.
            uint32_t        max_address;   // The maximum address the heap can be expanded to.
            bool            is_kernel;     // Should extra pages requested by us be mapped as supervisor-only?
            bool            rw;             // Should extra pages requested by us be mapped as read-only?
        } __PACKED;

        typedef FastChunk   fast_chunk_t;
        typedef FastBin     fast_bin_t;
        typedef BigChunk    big_chunk_t;
        typedef Heap        heap_t;

        void initialize(uint32_t start, uint32_t end, uint32_t size, bool is_kernel, bool rw);

        void* malloc(uint32_t size);

        void free(void* p);

    NAMESPACE_END(memory_manager)

NAMESPACE_END(kernel)

#endif // KERNEL_MEMORY_MANAGER_H