#include <include/dtables_structs.h>
#include <include/dtables.h>

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

/*
    This template function will build 255 different functions, each bulding a function that handles
    an interrupt.
*/

static void idt::_init()
{
    idt_ptr.size = (sizeof(idt::entry_t) * 256) - 1;
    idt_ptr.base = (uint32_t)&idt_entries;

    // The CPU has 256 different interrupts;
    uint32_t p = (uint32_t)isr_0;
    const auto dif = ((uint32_t)isr_1) - ((uint32_t)isr_0);
    for (int i = 0; i < 32; ++i) {
        idt::_config_entry(i, p, 0x08, 0x8E);
        p += dif;
    }

    idt::_config_entry(0, reinterpret_cast<uint32_t>(isr_0), 0x08, 0x8E);
    idt::_config_entry(1, reinterpret_cast<uint32_t>(isr_1), 0x08, 0x8E);
    idt::_config_entry(2, reinterpret_cast<uint32_t>(isr_2), 0x08, 0x8E);
    idt::_config_entry(3, reinterpret_cast<uint32_t>(isr_3), 0x08, 0x8E);
    idt::_config_entry(4, reinterpret_cast<uint32_t>(isr_4), 0x08, 0x8E);
    idt::_config_entry(5, reinterpret_cast<uint32_t>(isr_5), 0x08, 0x8E);
    idt::_config_entry(6, reinterpret_cast<uint32_t>(isr_6), 0x08, 0x8E);
    idt::_config_entry(7, reinterpret_cast<uint32_t>(isr_7), 0x08, 0x8E);
    idt::_config_entry(8, reinterpret_cast<uint32_t>(isr_8), 0x08, 0x8E);
    idt::_config_entry(9, reinterpret_cast<uint32_t>(isr_9), 0x08, 0x8E);
    idt::_config_entry(10, reinterpret_cast<uint32_t>(isr_10), 0x08, 0x8E);
    idt::_config_entry(11, reinterpret_cast<uint32_t>(isr_11), 0x08, 0x8E);
    idt::_config_entry(12, reinterpret_cast<uint32_t>(isr_12), 0x08, 0x8E);
    idt::_config_entry(13, reinterpret_cast<uint32_t>(isr_13), 0x08, 0x8E);
    idt::_config_entry(14, reinterpret_cast<uint32_t>(isr_14), 0x08, 0x8E);
    idt::_config_entry(15, reinterpret_cast<uint32_t>(isr_15), 0x08, 0x8E);
    idt::_config_entry(16, reinterpret_cast<uint32_t>(isr_16), 0x08, 0x8E);
    idt::_config_entry(17, reinterpret_cast<uint32_t>(isr_17), 0x08, 0x8E);
    idt::_config_entry(18, reinterpret_cast<uint32_t>(isr_18), 0x08, 0x8E);
    idt::_config_entry(19, reinterpret_cast<uint32_t>(isr_19), 0x08, 0x8E);
    idt::_config_entry(20, reinterpret_cast<uint32_t>(isr_20), 0x08, 0x8E);
    idt::_config_entry(21, reinterpret_cast<uint32_t>(isr_21), 0x08, 0x8E);
    idt::_config_entry(22, reinterpret_cast<uint32_t>(isr_22), 0x08, 0x8E);
    idt::_config_entry(23, reinterpret_cast<uint32_t>(isr_23), 0x08, 0x8E);
    idt::_config_entry(24, reinterpret_cast<uint32_t>(isr_24), 0x08, 0x8E);
    idt::_config_entry(25, reinterpret_cast<uint32_t>(isr_25), 0x08, 0x8E);
    idt::_config_entry(26, reinterpret_cast<uint32_t>(isr_26), 0x08, 0x8E);
    idt::_config_entry(27, reinterpret_cast<uint32_t>(isr_27), 0x08, 0x8E);
    idt::_config_entry(28, reinterpret_cast<uint32_t>(isr_28), 0x08, 0x8E);
    idt::_config_entry(29, reinterpret_cast<uint32_t>(isr_29), 0x08, 0x8E);
    idt::_config_entry(30, reinterpret_cast<uint32_t>(isr_30), 0x08, 0x8E);
    idt::_config_entry(31, reinterpret_cast<uint32_t>(isr_31), 0x08, 0x8E);
    
    // PIC 
    idt::_config_entry(32, reinterpret_cast<uint32_t>(irq_0), 0x08, 0x8E);
    idt::_config_entry(33, reinterpret_cast<uint32_t>(irq_1), 0x08, 0x8E);
    idt::_config_entry(34, reinterpret_cast<uint32_t>(irq_2), 0x08, 0x8E);
    idt::_config_entry(35, reinterpret_cast<uint32_t>(irq_3), 0x08, 0x8E);
    idt::_config_entry(36, reinterpret_cast<uint32_t>(irq_4), 0x08, 0x8E);
    idt::_config_entry(37, reinterpret_cast<uint32_t>(irq_5), 0x08, 0x8E);
    idt::_config_entry(38, reinterpret_cast<uint32_t>(irq_6), 0x08, 0x8E);
    idt::_config_entry(39, reinterpret_cast<uint32_t>(irq_7), 0x08, 0x8E);
    idt::_config_entry(40, reinterpret_cast<uint32_t>(irq_8), 0x08, 0x8E);
    idt::_config_entry(41, reinterpret_cast<uint32_t>(irq_9), 0x08, 0x8E);
    idt::_config_entry(42, reinterpret_cast<uint32_t>(irq_10), 0x08, 0x8E);
    idt::_config_entry(43, reinterpret_cast<uint32_t>(irq_11), 0x08, 0x8E);
    idt::_config_entry(44, reinterpret_cast<uint32_t>(irq_12), 0x08, 0x8E);
    idt::_config_entry(45, reinterpret_cast<uint32_t>(irq_13), 0x08, 0x8E);
    idt::_config_entry(46, reinterpret_cast<uint32_t>(irq_14), 0x08, 0x8E);
    idt::_config_entry(47, reinterpret_cast<uint32_t>(irq_15), 0x08, 0x8E);


    idt_dump((uint32_t)&idt_ptr);
}

static void idt::_config_entry(int32_t entry, uint32_t base, uint16_t sel, uint8_t flags)
{
    idt_entries[entry].base_low     = base & 0xFFFF;
    idt_entries[entry].base_high    = (base >> 16) & 0xFFFF;

    idt_entries[entry].sel          = sel;
    idt_entries[entry].flags        = flags;  // | 0x60
}