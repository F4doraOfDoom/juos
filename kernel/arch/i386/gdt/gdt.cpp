#include <arch/i386/gdt.h>

Gdt::entry_t    gdt_entries[5]      = { 0 };
Gdt::PtrStruct  gdt_ptr             = { 0, 0 };

void Gdt::Initialize()
{
    LOG_S("GDT: ", "Initializing...\n");
    Gdt::_Init();
}

static void Gdt::_Init()
{
    gdt_ptr.size = (5 * sizeof(Gdt::entry_t)) - 1;
    gdt_ptr.base = (uint32_t)(&gdt_entries);

    Gdt::EditEntry(0, 0, 0, 0, 0);
    Gdt::EditEntry(1, 0, 0xFFFFFFFF, (uint8_t)Gdt::Segment_Access_Type::Kernel_Code, 0xCF);
    Gdt::EditEntry(2, 0, 0xFFFFFFFF, (uint8_t)Gdt::Segment_Access_Type::Kernel_Data, 0xCF);
    Gdt::EditEntry(3, 0, 0xFFFFFFFF, (uint8_t)Gdt::Segment_Access_Type::User_Code, 0xCF);
    Gdt::EditEntry(4, 0, 0xFFFFFFFF, (uint8_t)Gdt::Segment_Access_Type::User_Data, 0xCF);

    gdt_dump((uint32_t)&gdt_ptr);
}

static void Gdt::EditEntry(int32_t entry, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
{
    gdt_entries[entry].base_low     = (base & 0xFFFF);
    gdt_entries[entry].base_middle  = (base >> 16) & 0xFF;
    gdt_entries[entry].base_high    = (base >> 24) & 0xFF;

    gdt_entries[entry].limit_low    = (limit & 0xFFFF);
    gdt_entries[entry].granularity  = (limit >> 16) & 0x0F;
    
    gdt_entries[entry].granularity |= gran & 0xF0;
    gdt_entries[entry].access       = access;
}