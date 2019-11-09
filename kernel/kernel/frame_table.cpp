#include <kernel/paging.h>

using namespace kernel::paging;

FrameTable::frame_table_result_t kernel::paging::FrameTable::find_first()
{
    FrameTable::frame_table_result_t result;

    for(uint32_t i = 0; i < this->length; i++)
    {
        if (!this->frames[i].is_taken)
        {
            result.idx = i;
            return result;
        }
    }

    result.error = true;
    return result;
}

void FrameTable::set_at_addr(uint32_t addr)
{
    this->frames[addr / PAGE_SIZE].is_taken = true;
} 

kernel::paging::frame_table_t::FrameTable(uint32_t length)
{
    this->length = length;
    this->frames = (frame_t*)heap::allocate(length);
    
    for (uint32_t i = 0; i < length; i++)
    {
        this->frames[i] = Frame();
    }
}