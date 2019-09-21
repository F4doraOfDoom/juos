#include "descriptor_tables.h"
#include <kernel/tty.h>

Gdt::entry_t     gdt_entries[5];
Gdt::ptr_struct  gdt_ptr;

void Gdt::initialize()
{
    Gdt::init();
}

static void Gdt::init()
{
    gdt_ptr.size = (5 * sizeof(Gdt::entry_t)) - 1;
    gdt_ptr.base = (uint32_t)(&gdt_entries);

    config_entry(0, 0, 0, 0, 0);
    config_entry(1, 0, 0xFFFFFFFF, (uint8_t)Gdt::Segment_Access_Type::Kernel_Code, 0xCF);
    config_entry(2, 0, 0xFFFFFFFF, (uint8_t)Gdt::Segment_Access_Type::Kernel_Data, 0xCF);
    config_entry(3, 0, 0xFFFFFFFF, (uint8_t)Gdt::Segment_Access_Type::User_Code, 0xCF);
    config_entry(4, 0, 0xFFFFFFFF, (uint8_t)Gdt::Segment_Access_Type::User_Data, 0xCF);

    gdt_dump((uint32_t)&gdt_ptr);
}

static void Gdt::config_entry(int32_t entry, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
{
    gdt_entries[entry].base_low     = (base & 0xFFFF);
    gdt_entries[entry].base_middle  = (base >> 16) & 0xFF;
    gdt_entries[entry].base_high    = (base >> 24) & 0xFF;

    gdt_entries[entry].limit_low    = (limit & 0xFFFF);
    gdt_entries[entry].granularity  = (limit >> 16) & 0x0F;
    
    gdt_entries[entry].granularity |= gran & 0xF0;
    gdt_entries[entry].access       = access;
}