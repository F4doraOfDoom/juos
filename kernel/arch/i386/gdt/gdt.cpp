#include <arch/i386/gdt.h>

gdt::entry_t    gdt_entries[5]      = { 0 };
gdt::PtrStruct  gdt_ptr             = { 0, 0 };

void gdt::initialize()
{
    LOG_S("GDT: ", "Initializing...\n");
    gdt::_init();
}

static void gdt::_init()
{
    gdt_ptr.size = (5 * sizeof(gdt::entry_t)) - 1;
    gdt_ptr.base = (uint32_t)(&gdt_entries);

    gdt::edit_entry(0, 0, 0, 0, 0);
    gdt::edit_entry(1, 0, 0xFFFFFFFF, (uint8_t)gdt::Segment_Access_Type::Kernel_Code, 0xCF);
    gdt::edit_entry(2, 0, 0xFFFFFFFF, (uint8_t)gdt::Segment_Access_Type::Kernel_Data, 0xCF);
    gdt::edit_entry(3, 0, 0xFFFFFFFF, (uint8_t)gdt::Segment_Access_Type::User_Code, 0xCF);
    gdt::edit_entry(4, 0, 0xFFFFFFFF, (uint8_t)gdt::Segment_Access_Type::User_Data, 0xCF);

    gdt_dump((uint32_t)&gdt_ptr);
}

static void gdt::edit_entry(int32_t entry, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
{
    gdt_entries[entry].base_low     = (base & 0xFFFF);
    gdt_entries[entry].base_middle  = (base >> 16) & 0xFF;
    gdt_entries[entry].base_high    = (base >> 24) & 0xFF;

    gdt_entries[entry].limit_low    = (limit & 0xFFFF);
    gdt_entries[entry].granularity  = (limit >> 16) & 0x0F;
    
    gdt_entries[entry].granularity |= gran & 0xF0;
    gdt_entries[entry].access       = access;
}