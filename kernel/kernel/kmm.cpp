#include <kernel/kmm.h>

#define SET_IS_PRESENT(chunk, type)     (chunk->prev = (type*)((uintptr_t)chunk->prev | ((uintptr_t)0x1)))
#define CHECK_IS_PRESENT(chunk)   ((uintptr_t)chunk->prev & ((uintptr_t)0x1))

#define FASTBIN_SET_PRESENT(chunk) SET_IS_PRESENT(chunk, fast_chunk_t)
#define FASTBIN_CHK_PRESENT(chunk) CHECK_IS_PRESENT(chunk, fast_chunk_t)

#define SLOWBIN_SET_PRESENT(chunk) SET_IS_PRESENT(chunk, big_chunk_t)
#define SLOWBIN_CHK_PRESENT(chunk) CHECK_IS_PRESENT(chunk, big_chunk_t)

#define CHUNK_IS_PRESENT(chunk) bitset::test((uint32_t*)chunk->prev, IS_PRESENT_BIT)

using namespace kernel::memory_manager;

static std::static_stack<fast_chunk_t, FASTBIN_MAX_SIZE> __fastbin_stack;
static heap_t* __mapped_heap;
static uint32_t _fastbin_area_end;

void kernel::memory_manager::initialize(uint32_t start, uint32_t end, uint32_t max, bool is_kernel, bool rw)
{

    LOG_S("MEMORY MANAGER: ", "Initializing...\n");

    LOG_SA("MEMORY MANAGER: ", "Initializing heap... start: %p, end: %p\n", start, end);
    __mapped_heap = (heap_t*)heap::allocate(sizeof(memory_manager::heap_t)); 
    memset(__mapped_heap, 0, sizeof(heap_t));

    ASSERT(IS_ALIGNED(start) && IS_ALIGNED(end));

    _fastbin_area_end = (start + 
        (sizeof(fast_chunk_t) + FASTBIN_THRESHOLD) * FASTBIN_MAX_SIZE);
    __mapped_heap->fast_bins        = (fast_chunk_t*)start;
    __mapped_heap->slow_bins        = (big_chunk_t*)_fastbin_area_end;
    __mapped_heap->start_address    = start;
    __mapped_heap->end_address      = end;
    __mapped_heap->max_address      = max;
    __mapped_heap->is_kernel        = is_kernel;
    __mapped_heap->rw               = rw;

    auto heap_mapping = paging::_HeapMappingSettings{start, end};
    kernel::paging::initialize(&heap_mapping);
 
}

static void* __get_fastbin()
{
    if (!__fastbin_stack.is_empty())
    {
        fast_chunk_t new_chunk;
        new_chunk.ptr_to_heap = (uint32_t*)(__mapped_heap->fast_bins + (__fastbin_stack.size() * FASTBIN_THRESHOLD));

        __fastbin_stack.push(new_chunk);
        return new_chunk.ptr_to_heap;
    }

    return nullptr;
}

void* kernel::memory_manager::malloc(size_t size)
{
    if (size <= FASTBIN_THRESHOLD)
    {
        return __get_fastbin();
    }
}