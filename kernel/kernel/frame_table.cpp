#include <kernel/paging.h>

using namespace kernel::paging;

FrameTable::frame_table_result_t kernel::paging::FrameTable::FindFirst()
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

void FrameTable::SetAtAddress(uint32_t addr)
{
    this->frames[addr / PAGE_SIZE].is_taken = true;
} 

void FrameTable::SetAtIndex(uint32_t idx)
{
    this->frames[idx].is_taken = true;
}

void FrameTable::FreeAtIndex(uint32_t idx)
{
    this->frames[idx].is_taken = false;
}

kernel::paging::frame_table_t::FrameTable(uint32_t length)
{
    this->length = length;
    this->frames = (frame_t*)heap::Allocate(length);
    
    for (uint32_t i = 0; i < length; i++)
    {
        this->frames[i] = Frame();
    }
}