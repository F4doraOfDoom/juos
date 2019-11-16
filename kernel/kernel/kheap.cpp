#include <kernel/kheap.h>

extern uint32_t end; // defined by linker
// the primitive heap cannot be freed
uint32_t __primitive_heap = (uint32_t)&end; 

uint32_t kernel::heap::allocate_p(size_t n, uint32_t* phys, bool alligned)
{
#ifdef K_LOG_MALLOC
    LOG_SA("PRIMITIVE HEAP: ", "Allocating %d bytes\n", n);
#endif

    if (alligned)
    {
        if (!IS_ALIGNED(__primitive_heap))
        {
            __primitive_heap &= HEAP_ALIGNMENT;
            __primitive_heap += PAGE_SIZE;
        }
    } 

    if (phys != nullptr)
    {
        *phys = __primitive_heap;
    }

    uint32_t tmp = __primitive_heap;
    __primitive_heap += n;

    // substract _n_ to return beginning
    return tmp;
}

uint32_t kernel::heap::allocate(size_t n, bool alligned)
{
    return allocate_p(n, nullptr, alligned);
}