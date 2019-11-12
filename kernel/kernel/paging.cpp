#include <kernel/paging.h>
#include <kernel/kmm.h>

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

static page_t* _get_page(uint32_t addr, page_directory_t* dir, bool make_page)
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

static uint32_t _map_virt_to_phys(uint32_t start, uint32_t& end, page_directory_t* dir)
{
    uint32_t pages_created = 0;

    LOG_SA("PAGING: ", "Identity mapping regions %d (%p) to %d (%p)...\n", start, start, end, end);
    for(uint32_t top = start; top < end; pages_created++, top += PAGE_SIZE)
    {
        auto res = frame_table.find_first();
        auto page = _get_page(top, dir, true);
        
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

    return pages_created;
}

void kernel::paging::initialize(_HeapMappingSettings* _heap_mapping)
{
    LOG_S("PAGING: ", "Initializing...\n");

    auto number_of_frames = K_PHYSICAL_MEM_SIZE / FRAME_SIZE;
    uint32_t page_idx = 0;

    page_directory_t* kernel_directory = (page_directory_t*)heap::allocate(sizeof(page_directory_t));
    memset((char*)kernel_directory, '\0', sizeof(page_directory_t));

    frame_table = FrameTable(number_of_frames);    

    // identity map the kernel
    auto pages_created = _map_virt_to_phys(0, __kernel_heap, kernel_directory);

    if (_heap_mapping)
    {
        // identity map the heap
        uint32_t heap_end = K_HEAP_START + K_HEAP_INITIAL_SIZE;
        pages_created += _map_virt_to_phys(K_HEAP_START, heap_end, kernel_directory);
    }

    LOG_SA("PAGING: ", "Created %d pages.\n", pages_created);

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
    "Faulting address: %x\n"
    "Page isnt present: %d\n"
    "Invalid write operation: %d\n"
    "Access from user-mode: %d\n"
    "Overwritten CPU reserved bits: %d\n"
    "Caused by instruction fetch: %d\n",
    faulting_address,
    present,
    rw,
    us,
    reserved,
    ifetch);
}