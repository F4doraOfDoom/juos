#include <kernel/kmm.h>

// #define SET_IS_PRESENT(chunk, type)     (chunk->prev = (type*)((uintptr_t)chunk->prev | ((uintptr_t)0x1)))
// #define CHECK_IS_PRESENT(chunk)   ((uintptr_t)chunk->prev & ((uintptr_t)0x1))

// #define FASTBIN_SET_PRESENT(chunk) SET_IS_PRESENT(chunk, fast_chunk_t)
// #define FASTBIN_CHK_PRESENT(chunk) CHECK_IS_PRESENT(chunk, fast_chunk_t)

// #define SLOWBIN_SET_PRESENT(chunk) SET_IS_PRESENT(chunk, big_chunk_t)
// #define SLOWBIN_CHK_PRESENT(chunk) CHECK_IS_PRESENT(chunk, big_chunk_t)

// #define CHUNK_IS_PRESENT(chunk) bitset::test((uint32_t*)chunk->prev, IS_PRESENT_BIT)

#define SET_FASTBIN(ptr)    (uint32_t*)((uintptr_t)ptr | (uintptr_t)IS_FASTBIN_BIT)    
#define CHK_FASTBIN(ptr)    (uint32_t*)((uintptr_t)ptr & (uintptr_t)IS_FASTBIN_BIT)
#define MSK_FASTBIN(ptr)    (uint32_t*)((uintptr_t)ptr ^ (uintptr_t)IS_FASTBIN_BIT)

using namespace kernel::memory_manager;

extern uint32_t __primitive_heap;

// TODO: Add support for multiple fast bins of different sizes!
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

    __mapped_heap->fast_bins        = (uint32_t*)start;
    __mapped_heap->slow_bins        = (uint32_t*)_fastbin_area_end;
    __mapped_heap->start_address    = start;
    __mapped_heap->end_address      = end;
    __mapped_heap->max_address      = max;
    __mapped_heap->is_kernel        = is_kernel;
    __mapped_heap->rw               = rw;

    paging::_HeapMappingSettings heap_mapping = paging::_HeapMappingSettings{start, end};
    kernel::paging::initialize(&heap_mapping);    
}

/**
 * @brief Allocate a small chunk (defined by FASTBIN_THRESHOLD)
 * The chunk will be taken from a premade pool of chunks (called a bin)
 * allowing for a fast allocation at runtime when requesting a small 
 * chunk
 * 
 * @return void* - pointer to area on the heap
 */
static void* __malloc_fastbin()
{
    uint32_t* addr = nullptr;

    // lets go over each chunk in the pool (bin) and see if its free
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
            //heap_addr = SET_FASTBIN(heap_addr);    // so we'll be able to tell if its a fast bin or not
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

static void* __malloc_big()
{
    uint32_t* curr = __mapped_heap->slow_bins;
    
     
}

/**
 * @brief Free a chunk allocated by __malloc_fastbin,
 * allowing for future allocations to take the place of 
 * the allocated chunk
 * 
 * @param ptr - pointer to chunk to deallocate
 */
static void __free_fastbin(void* ptr)
{
    // we calculate the index of the chunk, so we'll know where to free
    const uint32_t chunk_idx = (uint32_t)(
        ((uint32_t*)ptr - ((uint32_t*)__mapped_heap->fast_bins))
        / (FASTBIN_THRESHOLD / 4)
    );

#ifdef K_LOG_MALLOC
    LOG_SA("FREE - FASTBIN: ", "Freeing chunk at addr %p (index %d)\n", ptr, chunk_idx);
#endif

    __fast_bin->chunks[chunk_idx].ptr_to_heap = nullptr;
}

/**
 * @brief Free a chunk allocated by __malloc_big,
 * allowing for future allocations to take the place of 
 * the allocated chunk
 * 
 * @param ptr - pointer to chunk to deallocate
 */
static void __free_big(void* ptr)
{

}

void* kernel::memory_manager::malloc(size_t size)
{
    // treat negative requests as zero
    if (size <= 0) return nullptr;

    if (size <= FASTBIN_THRESHOLD)
    {
        return __malloc_fastbin();
    }
    else
    {
        return __malloc_big();
    }
}

void kernel::memory_manager::free(void* ptr)
{
    // we know a chunk is a fast bin if it is in the fastbin region
    if (IN_RANGE_C(ptr, __mapped_heap->fast_bins, __mapped_heap->slow_bins))
    {
        __free_fastbin(ptr);
    }
    else
    {
        __free_big(ptr);
    }
}