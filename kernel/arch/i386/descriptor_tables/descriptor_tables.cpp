#include <include/descriptor_tables_structs.h>
#include <include/descriptor_tables.h>

Gdt::entry_t    gdt_entries[5]      = { 0 };
Gdt::PtrStruct  gdt_ptr             = { 0, 0 };
Idt::entry_t    idt_entries[256]    = { 0 };
Idt::PtrStruct  idt_ptr             = { 0, 0 };


void Gdt::initialize()
{
    Gdt::_init();
}

void Idt::initialize()
{
    Idt::_init();
}

static void Gdt::_init()
{
    gdt_ptr.size = (5 * sizeof(Gdt::entry_t)) - 1;
    gdt_ptr.base = (uint32_t)(&gdt_entries);

    Gdt::_config_entry(0, 0, 0, 0, 0);
    Gdt::_config_entry(1, 0, 0xFFFFFFFF, (uint8_t)Gdt::Segment_Access_Type::Kernel_Code, 0xCF);
    Gdt::_config_entry(2, 0, 0xFFFFFFFF, (uint8_t)Gdt::Segment_Access_Type::Kernel_Data, 0xCF);
    Gdt::_config_entry(3, 0, 0xFFFFFFFF, (uint8_t)Gdt::Segment_Access_Type::User_Code, 0xCF);
    Gdt::_config_entry(4, 0, 0xFFFFFFFF, (uint8_t)Gdt::Segment_Access_Type::User_Data, 0xCF);

    gdt_dump((uint32_t)&gdt_ptr);
}

static void Gdt::_config_entry(int32_t entry, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
{
    gdt_entries[entry].base_low     = (base & 0xFFFF);
    gdt_entries[entry].base_middle  = (base >> 16) & 0xFF;
    gdt_entries[entry].base_high    = (base >> 24) & 0xFF;

    gdt_entries[entry].limit_low    = (limit & 0xFFFF);
    gdt_entries[entry].granularity  = (limit >> 16) & 0x0F;
    
    gdt_entries[entry].granularity |= gran & 0xF0;
    gdt_entries[entry].access       = access;
}


template <uint32_t N>
static void Idt::_request_isr()
{
    const int32_t interrupt_num = N; 
    int32_t _;

    asm volatile(
        "cli\n"
        "pushl 0\n" 
        "pushl %1\n"
        "jmp isr_basic_caller\n"
        : "=r"(_)
        : "r"(interrupt_num)
    );
}

static void Idt::_init()
{
    idt_ptr.size = (sizeof(Idt::entry_t) * 256) - 1;
    idt_ptr.base = (uint32_t)&idt_entries;

    Idt::_config_entry(0, reinterpret_cast<uint32_t>(_request_isr<0>), 0x08, 0x8E);
    Idt::_config_entry(1, reinterpret_cast<uint32_t>(_request_isr<1>), 0x08, 0x8E);
    Idt::_config_entry(2, reinterpret_cast<uint32_t>(_request_isr<2>), 0x08, 0x8E);
    Idt::_config_entry(3, reinterpret_cast<uint32_t>(_request_isr<3>), 0x08, 0x8E);

    idt_dump((uint32_t)&idt_ptr);
}

static void Idt::_config_entry(int32_t entry, uint32_t base, uint16_t sel, uint8_t flags)
{
    idt_entries[entry].base_low     = base & 0xFFFF;
    idt_entries[entry].base_high    = (base >> 16) & 0xFFFF;

    idt_entries[entry].sel          = sel;
    idt_entries[entry].flags        = flags;  // | 0x60
}