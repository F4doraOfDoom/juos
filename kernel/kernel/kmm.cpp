#include <kernel/kmm.h>

// #define SET_IS_PRESENT(chunk, type)     (chunk->prev = (type*)((uintptr_t)chunk->prev | ((uintptr_t)0x1)))
// #define CHECK_IS_PRESENT(chunk)   ((uintptr_t)chunk->prev & ((uintptr_t)0x1))

// #define FASTBIN_SET_PRESENT(chunk) SET_IS_PRESENT(chunk, fast_chunk_t)
// #define FASTBIN_CHK_PRESENT(chunk) CHECK_IS_PRESENT(chunk, fast_chunk_t)

// #define SLOWBIN_SET_PRESENT(chunk) SET_IS_PRESENT(chunk, big_chunk_t)
// #define SLOWBIN_CHK_PRESENT(chunk) CHECK_IS_PRESENT(chunk, big_chunk_t)

// #define CHUNK_IS_PRESENT(chunk) bitset::test((uint32_t*)chunk->prev, IS_PRESENT_BIT)

using namespace kernel::MemoryManager;

#define BIGCHUNK_MIN_SPACE  (sizeof(big_chunk_t) + FASTBIN_THRESHOLD + 1)

inline void* BIGCHUNK_DATA_BEGIN(const big_chunk_t* chunk)
{
    return (void*)(chunk + 1);
}

inline void* BIGCHUNK_DATA_BEGIN(const void* chunk)
{
    return (void*)((uint8_t*)chunk + sizeof(big_chunk_t));
}


extern uint32_t __primitive_heap;

// TODO: Add support for multiple fast bins of different sizes!
//static std::static_stack<fast_chunk_t, FASTBIN_MAX_SIZE> __fastbin_stack;
static fast_bin_t*      __fast_bin          = nullptr;
static uint32_t         _fastbin_area_end   = 0;
static heap_t*          __mapped_heap       = nullptr;


void kernel::MemoryManager::Initialize(uint32_t start, uint32_t end, uint32_t max, bool is_kernel, bool rw)
{
    LOG_S("MEMORY MANAGER: ", "Initializing...\n");

    __fast_bin = (fast_bin_t*)heap::Allocate(sizeof(fast_bin_t));
    memset(__fast_bin, 0, sizeof(fast_bin_t));

    __fast_bin->start_addr = start;
    __fast_bin->end_addr = (start + 
        (sizeof(fast_chunk_t) + FASTBIN_THRESHOLD) * FASTBIN_MAX_SIZE);

    // we need to reserve extra space on the primitive heap for our fastbin chunks
    // to do that we grow the primitive heap, so it will be identity mapped when 
    // paging is initiated 
    const uint32_t reserve_memory_for_fastbins = __fast_bin->end_addr - __fast_bin->start_addr;
    _fastbin_area_end = start + reserve_memory_for_fastbins;
    //__primitive_heap += reserve_memory_for_fastbins;

    __fast_bin->chunks = (fast_chunk_t*)heap::Allocate(reserve_memory_for_fastbins);
    __fast_bin->bin_size       = FASTBIN_MAX_SIZE;
    __fast_bin->chunk_size     = FASTBIN_THRESHOLD;
    for(uint32_t i = 0; i < __fast_bin->bin_size; i++)
    {
        __fast_bin->chunks[i].ptr_to_heap = nullptr;
    }

#if CHECK_LOG_LEVEL(K_LOG_GENERAL, 1)
   LOG_SA("MEMORY MANAGER: ", "Initializing heap... start: %p, end: %p\n", start, end);
#endif

    __mapped_heap = (heap_t*)heap::Allocate(sizeof(MemoryManager::heap_t)); 
    memset(__mapped_heap, 0, sizeof(heap_t));

    ASSERT(IS_ALIGNED(start) && IS_ALIGNED(end));

    __mapped_heap->fast_bins        = (uint32_t*)start;
    __mapped_heap->big_chunks       = (big_chunk_t*)_fastbin_area_end;
    __mapped_heap->allocated_chunks = 0;
    __mapped_heap->start_address    = start;
    __mapped_heap->end_address      = end;
    __mapped_heap->max_address      = max;
    __mapped_heap->is_kernel        = is_kernel;
    __mapped_heap->rw               = rw;

    // clear the first chunk
    memset(__mapped_heap->big_chunks, 0, sizeof(big_chunk_t));

    paging::_HeapMappingSettings heap_mapping = paging::_HeapMappingSettings{start, end, paging::_HeapMappingSettings::Type::HEAP};
    kernel::paging::Initialize(&heap_mapping);    
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
            __fast_bin->chunks[i].ptr_to_heap = (uint32_t*)heap_addr;
            addr = (uint32_t*)heap_addr;

            break;
        }
    }

#if CHECK_LOG_LEVEL(K_LOG_MM, MM_LOG_ALLOCATIONS)
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

static void* __malloc_big(uint32_t req_size)
{
    big_chunk_t* curr_chunk = __mapped_heap->big_chunks;
    big_chunk_t* smallest_hole = nullptr;
    uint32_t  smallest_hole_value = -1; // max uint32_t value
    big_chunk_t* ret = nullptr;

    //req_size += sizeof(big_chunk_t);

    // no chunks are allocated
    if (!__mapped_heap->allocated_chunks)
    {
        curr_chunk->size = req_size;
        curr_chunk->used = true;
        ret = curr_chunk;
    }
    // there are chunks to search
    else
    {
        // try to find the smallest hole
        while(curr_chunk->next)
        {
            if (!curr_chunk->used)
            {
                uint32_t size = (uint32_t)PTR_SUB(curr_chunk->next, curr_chunk);//curr_chunk->next - curr_chunk; 
                // make sure it is indeed the smallest hole, and also satisifes
                // the requested amount of memory
                if (size < smallest_hole_value && size >= req_size)
                {
                    smallest_hole = curr_chunk;
                    smallest_hole_value = req_size;
                    break;
                }
            }

            curr_chunk = curr_chunk->next;
        }

        // not all chunks were used, we found a hole!
        if (smallest_hole)
        {
            smallest_hole->used = true;
            ret = smallest_hole; 

            // the hole is bigger than the requested size and,
            // we can allocate another chunk with the remaining space
            if (smallest_hole_value - BIGCHUNK_MIN_SPACE > req_size)
            {
                big_chunk_t* unused_chunk = (big_chunk_t*)PTR_ADVANCE(ret, req_size); //((void*)ret + req_size);
                memset(unused_chunk, 0, sizeof(big_chunk_t));

                unused_chunk->size = (uint32_t)PTR_SUB(ret->next, ret) - req_size; //(ret->next - ret) - req_size;
                
                big_chunk_t* tmp = ret->next;
                ret->next = unused_chunk;
                tmp->prev = unused_chunk;
                unused_chunk->prev = ret;
                unused_chunk->next = tmp;
            }
            else
            {
                ret->size = ret->next - ret - 1;
            }
        }
        // all chunks were used; append another chunk at the end
        else
        {
            void* new_chunk_location = PTR_ADVANCE(curr_chunk, curr_chunk->size);//((uint32_t*)curr_chunk) + curr_chunk->size;
            // size for header
            // new_chunk_location = PTR_ADVANCE(new_chunk_location, sizeof(big_chunk_t));

            // clear any garbage found
            memset(new_chunk_location, 0, sizeof(big_chunk_t));

            big_chunk_t* new_chunk = (big_chunk_t*)new_chunk_location;

            curr_chunk->next = new_chunk;
            new_chunk->prev = curr_chunk;
            new_chunk->used = true;
            new_chunk->size = req_size;

            ret =  new_chunk;
        }
    }


#if CHECK_LOG_LEVEL(K_LOG_MM, MM_LOG_ALLOCATIONS)
    if (ret)
    {
        LOG_SA("MALLOC - BIGCHUNK: ", "Allocated chunk at %d (%x)\n", ret, ret);
    }
    else
    {
        LOG_S("MALLOC - BIGCHUNK: ", "Failed to allocate chunk!\n")
    }
#endif

#if CHECK_LOG_LEVEL(K_LOG_MM, MM_LOG_DUMPS)
    LOG_SA("MALLOC - BIG CHUNK DUMP:\n", 
    "Prev: %p,\nNext:%p,\nSize: %d,\nUsed: %d\n", 
    ret->prev,
    ret->next,
    ret->size,
    ret->used)
#endif

    if (ret)
    {
        __mapped_heap->allocated_chunks++;
    }

    return BIGCHUNK_DATA_BEGIN(ret);
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
    const uint32_t chunk_idx = 
        (uint32_t)PTR_SUB(ptr, __mapped_heap->fast_bins)
        / (FASTBIN_THRESHOLD);

// #if CHECK_LOG_LEVEL(K_LOG_MM, K_LOG_ALLOCATIONS)
//     LOG_SA("FREE - FASTBIN: ", "Freeing chunk at addr %p (index %d)\n", ptr, chunk_idx);
// #endif

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
    big_chunk_t* chunk = (big_chunk_t*)PTR_ADVANCE(ptr, -sizeof(big_chunk_t)); //(big_chunk_t*)(ptr - sizeof(big_chunk_t));

#if CHECK_LOG_LEVEL(K_LOG_MM, MM_LOG_CONSOLIDATION)
    bool consolidated_forwards = false;
    bool consolidated_backwards = false;
#endif

    chunk->used = false;

    // consolidate forwards
    if (chunk->next && !chunk->next->used)
    {
#if CHECK_LOG_LEVEL(K_LOG_MM, MM_LOG_CONSOLIDATION)
        consolidated_forwards = true;
#endif
        auto tmp = chunk->next;

        // remove the next block from the linked list
        chunk->next = tmp->next;
        if (tmp->next)
        {
            tmp->next->prev = chunk;
        }

        chunk->size += tmp->size + sizeof(BigChunk);
    }

    // consolidate backwards
    if (chunk->prev && !chunk->prev->used)
    {
#if CHECK_LOG_LEVEL(K_LOG_MM, MM_LOG_CONSOLIDATION)
        consolidated_backwards = true;
#endif
        auto tmp = chunk->prev;

        // remove the next block from the linked list
        chunk->prev = tmp->prev;
        if (tmp->prev)
        {
            tmp->prev->next = chunk;
        }

        chunk->size += tmp->size + sizeof(BigChunk);
    }

#if CHECK_LOG_LEVEL(K_LOG_MM, MM_LOG_CONSOLIDATION)
    LOG_SA("MALLOC - FREE: ", "Chunk %p: Forwards: %d, Backwards: %d\n", chunk, consolidated_forwards, consolidated_backwards);
#endif


    __mapped_heap->allocated_chunks--;
}

void* kernel::MemoryManager::malloc(size_t size)
{
    // treat negative requests as zero
    if (size <= 0) return nullptr;

    if (size <= FASTBIN_THRESHOLD)
    {
        return __malloc_fastbin();
    }
    else
    {
        // by aligning the size of the chunk to a power of 2,
        // we always have a few bits at the end of the "size"
        // to use to our needs. Thus, we can save a few bytes
        // of overhead
        size = ALIGN_VAL(size + sizeof(big_chunk_t), CHUNK_SIZE_ALIGN);
        return __malloc_big(size);
    }
}

void kernel::MemoryManager::free(void* ptr)
{
    // we know a chunk is a fast bin if it is in the fastbin region
    if (ptr >= __mapped_heap->fast_bins && ptr <= __mapped_heap->big_chunks)
    {
        __free_fastbin(ptr);
    }
    else
    {
        __free_big(ptr);
    }
}