/**
 * @file kheap.h
 * @author Jonathan Uklisty (F4doraOfDoom) (yoniu7@gmail.com)
 * @brief Internal implementation of the heap
 * @version 0.1
 * @date 01-11-2019
 * 
 * @copyright Copyright Jonathan Uklisty (c) 2019 (MIT License)
 * 
 */

#ifndef KERNEL_HEAP_H_
#define KERNEL_HEAP_H_

#include "kdef.h"

#include <allocator.hpp>
#include <stdint.h>

#define HEAP_ALIGNMENT 0xFFFFF000
#define PAGE_SIZE 0x1000

#define IS_ALIGNED(addr) ((addr & ~HEAP_ALIGNMENT) == 0)

NAMESPACE_BEGIN(kernel)

    NAMESPACE_BEGIN(heap)

        /**
         * @brief allocate _n_ bytes on the heap
         * 
         * @param n - number of bytes to allocate
         * @param alligned - whether or not the memory should be alligned to a 
         * page size - trye by default
         * @return uint32_t - pointer to allocated memory
         */
        uint32_t allocate(size_t n, bool alligned = true);

        /**
         * @brief allocate _n_ bytes on the heap
         * 
         * @param n - number of bytes to allocate
         * @param alligned - whether or not the memory should be alligned to a 
         * @param phys - pointer to the physical address 
         * page size - trye by default
         * @return uint32_t - pointer to allocated memory
         */
        uint32_t allocate_p(size_t n, uint32_t* phys, bool alligned = true);

    NAMESPACE_END(heap)

NAMESPACE_END(kernel)

#endif //KERNEL_HEAP_H_

