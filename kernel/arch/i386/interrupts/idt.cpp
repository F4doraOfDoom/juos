#include <arch/i386/idt.h>

Idt::entry_t    idt_entries[256]    = { 0 };
Idt::PtrStruct  idt_ptr             = { 0, 0 };

void Idt::Initialize()
{
    LOG_S("IDT: ", "Initializing...\n");
    Idt::_Init();
}

/*
    This template function will build 255 different functions, each bulding a function that handles
    an interrupt.
*/

void Idt::_Init()
{
    idt_ptr.size = (sizeof(Idt::entry_t) * 256) - 1;
    idt_ptr.base = (uint32_t)&idt_entries;

    // will Initialize the pic to have interrupts at 
    Pic::Initialize(0x20);

    _set_all_interrupts();

    idt_dump((uint32_t)&idt_ptr);
}


void Idt::EditEntry(int32_t entry, uint32_t base, uint16_t sel, uint8_t flags)
{
    idt_entries[entry].base_low     = base & 0xFFFF;
    idt_entries[entry].base_high    = (base >> 16) & 0xFFFF;

    idt_entries[entry].sel          = sel;
    idt_entries[entry].flags        = flags;  // | 0x60
}


void _set_all_interrupts()
{
    // 32 base CPU interrupts
    Idt::EditEntry(0, reinterpret_cast<uint32_t>(isr_0), 0x08, 0x8E);
    Idt::EditEntry(1, reinterpret_cast<uint32_t>(isr_1), 0x08, 0x8E);
    Idt::EditEntry(2, reinterpret_cast<uint32_t>(isr_2), 0x08, 0x8E);
    Idt::EditEntry(3, reinterpret_cast<uint32_t>(isr_3), 0x08, 0x8E);
    Idt::EditEntry(4, reinterpret_cast<uint32_t>(isr_4), 0x08, 0x8E);
    Idt::EditEntry(5, reinterpret_cast<uint32_t>(isr_5), 0x08, 0x8E);
    Idt::EditEntry(6, reinterpret_cast<uint32_t>(isr_6), 0x08, 0x8E);
    Idt::EditEntry(7, reinterpret_cast<uint32_t>(isr_7), 0x08, 0x8E);
    Idt::EditEntry(8, reinterpret_cast<uint32_t>(isr_8), 0x08, 0x8E);
    Idt::EditEntry(9, reinterpret_cast<uint32_t>(isr_9), 0x08, 0x8E);
    Idt::EditEntry(10, reinterpret_cast<uint32_t>(isr_10), 0x08, 0x8E);
    Idt::EditEntry(11, reinterpret_cast<uint32_t>(isr_11), 0x08, 0x8E);
    Idt::EditEntry(12, reinterpret_cast<uint32_t>(isr_12), 0x08, 0x8E);
    Idt::EditEntry(13, reinterpret_cast<uint32_t>(isr_13), 0x08, 0x8E);
    Idt::EditEntry(14, reinterpret_cast<uint32_t>(isr_14), 0x08, 0x8E);
    Idt::EditEntry(15, reinterpret_cast<uint32_t>(isr_15), 0x08, 0x8E);
    Idt::EditEntry(16, reinterpret_cast<uint32_t>(isr_16), 0x08, 0x8E);
    Idt::EditEntry(17, reinterpret_cast<uint32_t>(isr_17), 0x08, 0x8E);
    Idt::EditEntry(18, reinterpret_cast<uint32_t>(isr_18), 0x08, 0x8E);
    Idt::EditEntry(19, reinterpret_cast<uint32_t>(isr_19), 0x08, 0x8E);
    Idt::EditEntry(20, reinterpret_cast<uint32_t>(isr_20), 0x08, 0x8E);
    Idt::EditEntry(21, reinterpret_cast<uint32_t>(isr_21), 0x08, 0x8E);
    Idt::EditEntry(22, reinterpret_cast<uint32_t>(isr_22), 0x08, 0x8E);
    Idt::EditEntry(23, reinterpret_cast<uint32_t>(isr_23), 0x08, 0x8E);
    Idt::EditEntry(24, reinterpret_cast<uint32_t>(isr_24), 0x08, 0x8E);
    Idt::EditEntry(25, reinterpret_cast<uint32_t>(isr_25), 0x08, 0x8E);
    Idt::EditEntry(26, reinterpret_cast<uint32_t>(isr_26), 0x08, 0x8E);
    Idt::EditEntry(27, reinterpret_cast<uint32_t>(isr_27), 0x08, 0x8E);
    Idt::EditEntry(28, reinterpret_cast<uint32_t>(isr_28), 0x08, 0x8E);
    Idt::EditEntry(29, reinterpret_cast<uint32_t>(isr_29), 0x08, 0x8E);
    Idt::EditEntry(30, reinterpret_cast<uint32_t>(isr_30), 0x08, 0x8E);
    Idt::EditEntry(31, reinterpret_cast<uint32_t>(isr_31), 0x08, 0x8E);
}