#include <kernel/kmm.h>

using namespace kernel::memory_manager;

static heap_t* __mapped_heap;

void kernel::memory_manager::initialize(uint32_t start, uint32_t end, uint32_t max, bool is_kernel, bool rw)
{

    LOG_S("MEMORY MANAGER: ", "Initializing...\n");

    LOG_SA("MEMORY MANAGER: ", "Initializing heap... start: %p, end: %p\n", start, end);
    __mapped_heap = (heap_t*)heap::allocate(sizeof(memory_manager::heap_t)); 
    memset(__mapped_heap, 0, sizeof(heap_t));

    ASSERT(IS_ALIGNED(start) && IS_ALIGNED(end));

    __mapped_heap->head             = (chunk_t*)start;
    __mapped_heap->start_address    = start;
    __mapped_heap->end_address      = end;
    __mapped_heap->max_address      = max;
    __mapped_heap->is_kernel        = is_kernel;
    __mapped_heap->rw               = rw;

    auto heap_mapping = paging::_HeapMappingSettings{start, end};
    kernel::paging::initialize(&heap_mapping);
}

void* kernel::memory_manager::malloc(size_t size)
{
    chunk_t* current = __mapped_heap->head;
    
    if (!__mapped_heap->head)
    {
        chunk_t new_chunk;
        new_chunk.size = size + sizeof(chunk_t);
        memcpy(__mapped_heap->head, &new_chunk, sizeof(chunk_t));

        return __mapped_heap->head + sizeof(chunk_t); 
    }
}