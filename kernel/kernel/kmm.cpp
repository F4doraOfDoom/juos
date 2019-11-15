#include <kernel/kmm.h>

// #define SET_IS_PRESENT(chunk, type)     (chunk->prev = (type*)((uintptr_t)chunk->prev | ((uintptr_t)0x1)))
// #define CHECK_IS_PRESENT(chunk)   ((uintptr_t)chunk->prev & ((uintptr_t)0x1))

// #define FASTBIN_SET_PRESENT(chunk) SET_IS_PRESENT(chunk, fast_chunk_t)
// #define FASTBIN_CHK_PRESENT(chunk) CHECK_IS_PRESENT(chunk, fast_chunk_t)

// #define SLOWBIN_SET_PRESENT(chunk) SET_IS_PRESENT(chunk, big_chunk_t)
// #define SLOWBIN_CHK_PRESENT(chunk) CHECK_IS_PRESENT(chunk, big_chunk_t)

// #define CHUNK_IS_PRESENT(chunk) bitset::test((uint32_t*)chunk->prev, IS_PRESENT_BIT)

#define MARK_FASTBIN(ptr)   (uint32_t*)((uintptr_t)ptr | (uintptr_t)IS_FASTBIN_BIT)    
#define IS_FASTBIN(ptr)     (uint32_t*)((uintptr_t)ptr & (uintptr_t)IS_FASTBIN_BIT)

using namespace kernel::memory_manager;

extern uint32_t __primitive_heap;

//static std::static_stack<fast_chunk_t, FASTBIN_MAX_SIZE> __fastbin_stack;
static fast_bin_t*      __fast_bin          = nullptr;
static uint32_t         _fastbin_area_end   = 0;
static heap_t*          __mapped_heap       = nullptr;


void kernel::memory_manager::initialize(uint32_t start, uint32_t end, uint32_t max, bool is_kernel, bool rw)
{
    LOG_S("MEMORY MANAGER: ", "Initializing...\n");

    __fast_bin = (fast_bin_t*)heap::allocate(sizeof(fast_bin_t));
    memset(__fast_bin, 0, sizeof(fast_bin_t));

    __fast_bin->start_addr = start;
    __fast_bin->end_addr = (start + 
        (sizeof(fast_chunk_t) + FASTBIN_THRESHOLD) * FASTBIN_MAX_SIZE);

    // we need to reserve extra space on the primitive heap for our fastbin chunks
    // to do that we grow the primitive heap, so it will be identity mapped when 
    // paging is initiated 
    const uint32_t reserve_memory_for_fastbins = __fast_bin->end_addr - __fast_bin->start_addr;
    //__primitive_heap += reserve_memory_for_fastbins;

    __fast_bin->chunks = (fast_chunk_t*)heap::allocate(reserve_memory_for_fastbins);
    __fast_bin->bin_size       = FASTBIN_MAX_SIZE;
    __fast_bin->chunk_size     = FASTBIN_THRESHOLD;
    for(uint32_t i = 0; i < __fast_bin->bin_size; i++)
    {
        __fast_bin->chunks[i].ptr_to_heap = nullptr;
    }


    LOG_SA("MEMORY MANAGER: ", "Initializing heap... start: %p, end: %p\n", start, end);
    __mapped_heap = (heap_t*)heap::allocate(sizeof(memory_manager::heap_t)); 
    memset(__mapped_heap, 0, sizeof(heap_t));

    ASSERT(IS_ALIGNED(start) && IS_ALIGNED(end));

    __mapped_heap->fast_bins        = (fast_bin_t*)start;
    __mapped_heap->slow_bins        = (big_chunk_t*)_fastbin_area_end;
    __mapped_heap->start_address    = start;
    __mapped_heap->end_address      = end;
    __mapped_heap->max_address      = max;
    __mapped_heap->is_kernel        = is_kernel;
    __mapped_heap->rw               = rw;

    paging::_HeapMappingSettings heap_mapping = paging::_HeapMappingSettings{start, end};
    kernel::paging::initialize(&heap_mapping);    
}

static void* __malloc_fastbin()
{
    uint32_t* addr = nullptr;

    for (uint32_t i = 0; i < __fast_bin->bin_size; i++)
    {
        // find the first free chunk
        // chunk is free if it points to nowhere on the heap (a nullptr)
        if (__fast_bin->chunks[i].ptr_to_heap == nullptr)
        {
            const uint32_t* heap_addr = (((uint32_t*)__mapped_heap->fast_bins) + 
                                    (i * __fast_bin->chunk_size / 4));  // need to divide by 4 because we're casting 
                                                                        // to a uint32_t pointer, where the rules of 
                                                                        // arithmetic are WEIRD, man!

            __fast_bin->chunks[i].ptr_to_heap = (uint32_t*)heap_addr;
            addr = (uint32_t*)heap_addr;

            break;
        }
    }

#ifdef K_LOG_MALLOC
    if (addr)
    {
            LOG_SA("MALLOC - FASTBIN: ", "Allocated chunk at %d (%x)\n", addr, addr);
    }
    else
    {
        LOG_S("MALLOC - FASTBIN: ", "Failed to allocate chunk!\n")
    }
#endif

    return addr;
}

static void __free_fastbin(uint32_t* ptr)
{

}

void* kernel::memory_manager::malloc(size_t size)
{
    if (size <= FASTBIN_THRESHOLD)
    {
        return __malloc_fastbin();
    }
}

void kernel::memory_manager::free(void* ptr)
{
    if (IS_FASTBIN(ptr))
    {

    }
}