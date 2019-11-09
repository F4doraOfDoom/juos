#include <kernel/paging.h>

using namespace kernel::paging;

extern uint32_t __kernel_heap; // defined in kheap.cpp

FrameTable          frame_table;
page_directory_t*   current_directory   = nullptr;
uint32_t*           physical_pages      = nullptr;

// void PageTable::set_page_idx(uint32_t    idx,
//                 bool        is_present,
//                 bool        rw,
//                 bool        is_user,
//                 bool        was_accessed,
//                 bool        was_written,
//                 uint32_t    frame_addr)
// {
//     enteries[idx].is_present    = is_present;
//     enteries[idx].rw            = rw;
//     enteries[idx].is_user       = is_user;
//     enteries[idx].was_accessed  = was_accessed;
//     enteries[idx].was_written   = was_written;
//     enteries[idx].frame_addr    = frame_addr;
// }

static page_t* get_page(uint32_t addr, page_directory_t* dir, bool make_page)
{
    auto page_idx = addr / PAGE_SIZE;
    auto table_idx = page_idx / PAGE_TABLE_SIZE;

    if (dir->tables[table_idx])
    {
        return &dir->tables[table_idx]->enteries[page_idx % PAGE_TABLE_SIZE];
    }
    else if (make_page)
    {
        uint32_t page_addr = 0;
        dir->tables[table_idx] = (page_table_t*)kernel::heap::allocate_p(sizeof(page_table_t), &page_addr);
        memset(dir->tables[table_idx], 0, PAGE_SIZE);
        dir->table_addresses[table_idx] = page_addr | 0x7;
        
        return &dir->tables[table_idx]->enteries[page_addr%PAGE_TABLE_SIZE];
    }

    return nullptr;
} 

void kernel::paging::initialize()
{
    auto number_of_frames = K_PHYSICAL_MEM_SIZE;
    uint32_t page_idx = 0;

    page_directory_t* kernel_directory = (page_directory_t*)heap::allocate(sizeof(page_directory_t));
    memset((char*)kernel_directory, '\0', sizeof(page_directory_t));

    frame_table = FrameTable(number_of_frames);    

    LOG_S("PAGING: ", "Creating Pages\n");
    for(uint32_t top = 0; top < __kernel_heap; page_idx++, top += PAGE_SIZE)
    {
        auto res = frame_table.find_first();
        auto page = get_page(top, kernel_directory, true);
        
        if (res.error)
        {
            GO_PANIC("NO FUCKING FRAMES WTF", "");
        }

        page->is_present = true;
        page->is_user = false;
        page->rw = false;
        page->frame_addr = res.idx;        

        frame_table.set_at_addr(top);
    }

    LOG_SA("PAGING: ","%d pages created\n", page_idx);

    current_directory = kernel_directory;

    interrupts::set_handler(14, page_fault_handler);

    _load_page_directory((uint32_t*)&kernel_directory->table_addresses);
    _enable_paging();
}


void kernel::paging::page_fault_handler(void* regs_void)
{
    registers32_t* regs = (registers32_t*)regs_void;
    uint32_t faulting_address = 0;

    // faulting address gets stored in cr2
    asm volatile("mov %%cr2, %0" : "=r" (faulting_address));

    bool present    = !(regs->err_code & 0x1);
    bool rw         = regs->err_code & 0x2;
    bool us         = regs->err_code & 0x4;
    bool reserved   = regs->err_code & 0x8;
    bool ifetch     = regs->err_code & 0x10;


    GO_PANIC("PAGE FAULT!\n" 
    "Page isnt present: %d\n"
    "Invalid write operation: %d\n"
    "Access from user-mode: %d\n"
    "Overwritten CPU reserved bits: %d\n"
    "Caused by instruction fetch: %d\n",
    present,
    rw,
    us,
    reserved,
    ifetch);
}