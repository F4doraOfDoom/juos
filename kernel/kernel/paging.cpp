#include <kernel/paging.h>

using namespace kernel::paging;
using namespace kernel;

page_directory_t* kernel_directory  = nullptr;
page_directory_t* current_directory = nullptr;

// defined in page_frames.cpp
extern uint32_t* frames;
// defined in page_frames.cpp
extern uint32_t nframes; 
// defined in kheap.cpp
extern uint32_t __kernel_heap;

void kernel::paging::enable()
{
    _enable_paging();
}

void kernel::paging::start()
{
    // allocate array of frame indexes
    nframes = (K_PHYSICAL_MEM_SIZE / K_PAGE_SIZE);
    frames = (uint32_t*)heap::allocate(INDEX_FROM_BIT(nframes), false);
    memset(frames, 0, INDEX_FROM_BIT(nframes));


    // TODO: When I implement new, change this to calling the constrcutor of page_directory_t
    kernel_directory = (page_directory_t*)heap::allocate(sizeof(page_directory_t));
    memset(kernel_directory, 0, sizeof(page_directory_t));

    // we are currently using this directory
    current_directory = kernel_directory;

    uint32_t i = 0;
    while(i < __kernel_heap)
    {
        frame::alloc(get_page(
                i,
                kernel_directory,
                true
                ), // we want to create all pages in directory
            false, // this directory is the kernel's
            false // kernel directory is1 not writeable from user space
        );
        i += PAGE_SIZE;
    }

    interrupts::set_handler((uint32_t)InterruptList::PageFault, __page_fault_handler);

    // this will enable paging
    set_directory(kernel_directory);
    paging::enable();
}

void kernel::paging::set_directory(page_directory_t* dir)
{
    _load_page_directory((unsigned int*)dir);
}

page_t* kernel::paging::get_page(uint32_t address, page_directory_t* dir, bool create)
{
    address /= PAGE_SIZE;

    uint32_t table_idx = address / 1024;

    // if we find an existing table, return it
    if (dir->tables[table_idx] != nullptr)
    {
        return &dir->tables[table_idx]->pages[address%1024];
    }

    // if no, create one and return it
    if (create)
    {
        uint32_t table_ptr = 0;
        dir->tables[table_idx] = (page_table_t*)heap::allocate_p(
            sizeof(page_directory_t), 
            &table_ptr // save the physical address 
        );
        memset(dir->tables[table_idx], 0, PAGE_SIZE);
        dir->tables_physical[table_idx] = table_ptr | 0x7; // PRESENT, RW, US
        return &dir->tables[table_idx]->pages[address%1024];
    }

    return (page_t*)PAGE_NOT_FOUND;
}

void kernel::paging::__page_fault_handler(void*)
{
    GO_PANIC("PAGE FAULT!", "");
}