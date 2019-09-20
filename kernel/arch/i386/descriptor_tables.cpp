#include "descriptor_tables.h"
#include <kernel/tty.h>

gdt_entry_t     gdt_entries[5];
gdt_ptr_struct  gdt_ptr;

void initialize_gdt()
{
    init_gdt();
}

static void init_gdt()
{
    gdt_ptr.size = (5 * sizeof(gdt_entry_t)) - 1;
    gdt_ptr.base = (uint32_t)(&gdt_entries);

    terminal_writestring("Got here");
    set_gdt_stats(0, 0, 0, 0, 0);
    set_gdt_stats(1, 0, 0xFFFFFFFF, (uint8_t)Gdt_Segment_Access_Type::Kernel_Code, 0xCF);
    set_gdt_stats(2, 0, 0xFFFFFFFF, (uint8_t)Gdt_Segment_Access_Type::Kernel_Data, 0xCF);
    set_gdt_stats(3, 0, 0xFFFFFFFF, (uint8_t)Gdt_Segment_Access_Type::User_Code, 0xCF);
    set_gdt_stats(4, 0, 0xFFFFFFFF, (uint8_t)Gdt_Segment_Access_Type::User_Data, 0xCF);

    terminal_writestring("Also here");
    gdt_dump((uint32_t)&gdt_ptr);
}

static void set_gdt_stats(int32_t entry, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
{
    gdt_entries[entry].base_low     = (base & 0xFFFF);
    gdt_entries[entry].base_middle  = (base >> 16) & 0xFF;
    gdt_entries[entry].base_high    = (base >> 24) & 0xFF;

    gdt_entries[entry].limit_low    = (limit & 0xFFFF);
    gdt_entries[entry].granularity  = (limit >> 16) & 0x0F;
    
    gdt_entries[entry].granularity |= gran & 0xF0;
    gdt_entries[entry].access       = access;
}