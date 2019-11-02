#include <kernel/paging.h>

using namespace kernel::paging;

// defined in kheap.cpp
extern uint32_t __kernel_heap;

uint32_t* frames;
uint32_t nframes;

struct frame::_FrameInfo frame::get_info(uint32_t frame_addr)
{
    uint32_t frame = (frame_addr / PAGE_SIZE);
    uint32_t idx =  INDEX_FROM_BIT(frame);
    uint32_t off =  OFFSET_FROM_BIT(frame);

    return {(uint32_t*)frame, idx, off};
}

void frame::set(uint32_t frame_addr)
{
    auto [frame, idx, off] = frame::get_info(frame_addr);

    BIT_SET(frame, idx, off);
}

void frame::clear(uint32_t frame_addr)
{
    auto [frame, idx, off] = frame::get_info(frame_addr);

    BIT_CLEAR(frame, idx, off);
}

bool frame::test(uint32_t frame_addr)
{
    auto [frame, idx, off] = frame::get_info(frame_addr);

    return BIT_TEST(frame, idx, off);
}

uint32_t frame::find_first()
{
    for (uint32_t i = 0; i < INDEX_FROM_BIT(nframes); i++)
    {
        if (frames[i] == 0xFFFFFFFF)
        {
            for (uint32_t j = 0; j < BIT_FIELD_SIZE; j++)
            {
                uint32_t bit_to_test = 0x1 << j;
                if (!(frames[i] & bit_to_test)) 
                {
                    return i * BIT_FIELD_SIZE + j;
                }
            }
        }
    }

    return FRAME_NOT_FOUND;
}