#include <include/descriptor_tables_structs.h>
#include <include/descriptor_tables.h>

gdt::entry_t    gdt_entries[5]      = { 0 };
gdt::PtrStruct  gdt_ptr             = { 0, 0 };
idt::entry_t    idt_entries[256]    = { 0 };
idt::PtrStruct  idt_ptr             = { 0, 0 };


void gdt::initialize()
{
    gdt::_init();
}

void idt::initialize()
{
    idt::_init();
}

static void gdt::_init()
{
    gdt_ptr.size = (5 * sizeof(gdt::entry_t)) - 1;
    gdt_ptr.base = (uint32_t)(&gdt_entries);

    gdt::_config_entry(0, 0, 0, 0, 0);
    gdt::_config_entry(1, 0, 0xFFFFFFFF, (uint8_t)gdt::Segment_Access_Type::Kernel_Code, 0xCF);
    gdt::_config_entry(2, 0, 0xFFFFFFFF, (uint8_t)gdt::Segment_Access_Type::Kernel_Data, 0xCF);
    gdt::_config_entry(3, 0, 0xFFFFFFFF, (uint8_t)gdt::Segment_Access_Type::User_Code, 0xCF);
    gdt::_config_entry(4, 0, 0xFFFFFFFF, (uint8_t)gdt::Segment_Access_Type::User_Data, 0xCF);

    gdt_dump((uint32_t)&gdt_ptr);
}

static void gdt::_config_entry(int32_t entry, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
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
static void idt::_request_isr()
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

static void idt::_init()
{
    idt_ptr.size = (sizeof(idt::entry_t) * 256) - 1;
    idt_ptr.base = (uint32_t)&idt_entries;

    idt::_config_entry(0, reinterpret_cast<uint32_t>(_request_isr<0>), 0x08, 0x8E);
    idt::_config_entry(1, reinterpret_cast<uint32_t>(_request_isr<1>), 0x08, 0x8E);
    idt::_config_entry(2, reinterpret_cast<uint32_t>(_request_isr<2>), 0x08, 0x8E);
    idt::_config_entry(3, reinterpret_cast<uint32_t>(_request_isr<3>), 0x08, 0x8E);

    idt_dump((uint32_t)&idt_ptr);
}

static void idt::_config_entry(int32_t entry, uint32_t base, uint16_t sel, uint8_t flags)
{
    idt_entries[entry].base_low     = base & 0xFFFF;
    idt_entries[entry].base_high    = (base >> 16) & 0xFFFF;

    idt_entries[entry].sel          = sel;
    idt_entries[entry].flags        = flags;  // | 0x60
}