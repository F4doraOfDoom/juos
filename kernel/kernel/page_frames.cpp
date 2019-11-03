#include <kernel/paging.h>

using namespace kernel::paging;

// defined in kheap.cpp
extern uint32_t __kernel_heap;

uint32_t* frames;
uint32_t nframes;

void frame::alloc(page_t* page, bool is_user, bool is_writeable)
{
    if (page->frame_addr != 0)
    {
        // frame is already allocated
        return;
    }

    uint32_t idx = frame::find_first();
    if (idx == FRAME_NOT_FOUND)
    {
        GO_PANIC("NO FREE FRAMES!", "");
    }

    frame::set(idx * FRAME_SIZE);
    page->is_present = true;
    page->rw = is_writeable ? 1 : 0;
    page->is_user = is_user ? 0 : 1;
    page->frame_addr = idx;
}

void frame::dealloc(page_t* page)
{
    if (page->frame_addr == 0)
    {
        return; // the page doesnt have a frame
    }

    frame::clear(page->frame_addr);
    page->frame_addr = 0;
}

struct frame::_FrameInfo frame::get_info(uint32_t frame_addr)
{
    uint32_t frame = (frame_addr / PAGE_SIZE);
    uint32_t idx =  INDEX_FROM_BIT(frame);
    uint32_t off =  OFFSET_FROM_BIT(frame);

    return {idx, off};
}

void frame::set(uint32_t frame_addr)
{
    auto [idx, off] = frame::get_info(frame_addr);

    BIT_SET(frames, idx, off);
}

void frame::clear(uint32_t frame_addr)
{
    auto [idx, off] = frame::get_info(frame_addr);

    BIT_CLEAR(frames, idx, off);
}

bool frame::test(uint32_t frame_addr)
{
    auto [idx, off] = frame::get_info(frame_addr);

    return BIT_TEST(frames, idx, off);
}

uint32_t frame::find_first()
{
    for (uint32_t i = 0; i < INDEX_FROM_BIT(nframes); i++)
    {
        if (frames[i] != 0xFFFFFFFF)
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

    //return FRAME_NOT_FOUND;
}