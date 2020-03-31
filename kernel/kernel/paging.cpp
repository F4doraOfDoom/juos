#include <kernel/paging.h>
#include <kernel/kmm.h>

using namespace kernel::paging;

extern uint32_t __primitive_heap; // defined in kheap.cpp
static uint32_t __kernel_size;
static uint32_t __base_kernel_tables_size;

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

// pointer to a function that allocates memory
// takes the number of bytes to allocate and other optional arguments

static page_t* _get_page(uint32_t addr, page_directory_t* dir, bool make_page, MemoryAlloctor allocator)
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
        dir->tables[table_idx] = (page_table_t*)allocator(sizeof(page_table_t), &page_addr);
        memset(dir->tables[table_idx], 0, PAGE_SIZE);
        dir->table_addresses[table_idx] = page_addr | 0x7;
        
        return &dir->tables[table_idx]->enteries[page_addr%PAGE_TABLE_SIZE];
    }

    return nullptr;
} 

static uint32_t _map_virt_to_phys(uint32_t start, uint32_t& end, page_directory_t* dir, MemoryAlloctor allocator)
{
    uint32_t pages_created = 0;

#if CHECK_LOG_LEVEL(K_LOG_PAGING, PAGING_LOG_MAPPING)
    LOG_SA("PAGING: ", "Identity mapping regions %d (%p) to %d (%p)...\n", start, start, end, end);
#endif

    for(uint32_t top = start; top < end; pages_created++, top += PAGE_SIZE)
    {
        auto res = frame_table.FindFirst();
        auto page = _get_page(top, dir, true, allocator);
        
        if (res.error)
        {
            GO_PANIC("NO FUCKING FRAMES WTF", "");
        }

        page->is_present = true;
        page->is_user = false;
        page->rw = false;
        page->frame_addr = res.idx;        

        frame_table.SetAtIndex(res.idx);
    }

    return pages_created;
}

uint32_t kernel::paging::map_region(uint32_t start, uint32_t end, MemoryAlloctor allocator,  page_directory_t* dir)
{
    if (!dir)
    {
        dir = current_directory;
    }

    uint32_t pages_created = _map_virt_to_phys(start, end, dir, allocator);
    return pages_created;
}

void kernel::paging::SetDirectory(PageDirectory* directory)
{
   // DisableHardwareInterrupts();

    // assuming interrupts are already disabled
    memcpy(current_directory, directory, sizeof(PageDirectory));

    // EnableHardwareInterrupts();
}

void kernel::paging::Initialize(_HeapMappingSettings* heap_mapping)
{
    LOG_S("PAGING: ", "Initializing...\n");

    auto number_of_frames = K_PHYSICAL_MEM_SIZE / FRAME_SIZE * 10;
    
    page_directory_t* kernel_directory = (page_directory_t*)heap::Allocate(sizeof(page_directory_t));
    memset((char*)kernel_directory, '\0', sizeof(page_directory_t));

    frame_table = FrameTable(number_of_frames);    

    // remember the size of the kernel
    // for future constructions of page directories
    __kernel_size = __primitive_heap;

    // identity map the kernel
    auto allocator = [](uint32_t size, void* args) {
        return (void*)kernel::heap::Allocate_WPointer(size, (uint32_t*)args); 
    };
    auto pages_created = map_region(0, K_MAPPED_REGION, allocator, kernel_directory);

    uint32_t num_tables = pages_created / PAGE_TABLE_SIZE;
    __base_kernel_tables_size = (num_tables == 0 ? 1 : num_tables);

    if (heap_mapping)
    {
        // map the kernel's heap
        uint32_t heap_end = K_HEAP_START + K_HEAP_INITIAL_SIZE;
        pages_created += map_region(K_HEAP_START, heap_end, allocator, kernel_directory);
    }

#if CHECK_LOG_LEVEL(K_LOG_PAGING, PAGING_LOG_CREATION)
    LOG_SA("PAGING: ", "Created %d pages.\n", pages_created);
#endif

    current_directory = kernel_directory;

    Interrupts::set_handler(14, page_fault_handler);

    SetDirectory(kernel_directory);
    _load_page_directory((uint32_t*)&current_directory->table_addresses);
    _enable_paging();
}

PageDirectory* kernel::paging::create_directory(Vector<_HeapMappingSettings>& proc_mappings)
{
#ifdef K_LOG_PAGING
    LOG_S("PAGING: ", "Creating new page directory...\n")
#endif
    PageDirectory* new_directory = new PageDirectory();

    // we want to have a kernel that is mapped 1-1
    for (uint32_t i = 0; i < __base_kernel_tables_size; i++)
    {
        new_directory->tables[i]            = current_directory->tables[i];
        new_directory->table_addresses[i]   = current_directory->table_addresses[i];
    }

    for (const auto& mappings : proc_mappings)
    {
#ifdef K_LOG_PAGING
        LOG_SA("PAGING: ", "Mappings are %d->%d\n", mapping->begin, mapping->end);
#endif
        auto allocator = [](uint32_t size, void* args) {
            return (void*)kernel::heap::Allocate_WPointer(size, (uint32_t*)args); 
        };
    
        map_region(mappings.begin, mappings.end, allocator, new_directory);
    }

    return new_directory;
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
    "Invalid Write operation: %d\n"
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