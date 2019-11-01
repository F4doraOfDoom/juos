#include <kernel/kheap.h>

extern uint32_t end; // defined by linker
uint32_t __kernel_heap = (uint32_t)&end; // this will be the beginning of the kernel's heap

uint32_t kernel::memory::allocate_p(size_t n, uint32_t* phys, bool alligned)
{
    if (alligned & IS_ALIGNED(__kernel_heap))
    {
        __kernel_heap &= HEAP_ALIGNMENT;
        __kernel_heap += PAGE_SIZE;
    }

    if (phys != nullptr)
    {
        *phys = __kernel_heap;
    }

    __kernel_heap += n;

    // substract _n_ to return beginning
    return __kernel_heap - n;
}

uint32_t kernel::memory::allocate(size_t n, bool alligned)
{
    
    return allocate_p(n, nullptr, alligned);
}