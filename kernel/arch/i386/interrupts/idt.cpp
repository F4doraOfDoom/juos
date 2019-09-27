#include <include/idt.h>

idt::entry_t    idt_entries[256]    = { 0 };
idt::PtrStruct  idt_ptr             = { 0, 0 };

void idt::initialize()
{
    idt::_init();
}

/*
    This template function will build 255 different functions, each bulding a function that handles
    an interrupt.
*/

static void idt::_init()
{
    idt_ptr.size = (sizeof(idt::entry_t) * 256) - 1;
    idt_ptr.base = (uint32_t)&idt_entries;

    _pic_remap(0x20, 0x28);
    
    _set_all_interrupts();

    idt_dump((uint32_t)&idt_ptr);
}


static void idt::_config_entry(int32_t entry, uint32_t base, uint16_t sel, uint8_t flags)
{
    idt_entries[entry].base_low     = base & 0xFFFF;
    idt_entries[entry].base_high    = (base >> 16) & 0xFFFF;

    idt_entries[entry].sel          = sel;
    idt_entries[entry].flags        = flags;  // | 0x60
}

static void _pic_remap(uint8_t offset1, uint8_t offset2)
{
    unsigned char a1, a2;

    a1 = inb(PIC1_DATA);
    a2 = inb(PIC2_DATA);

    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
	outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
	outb(PIC1_DATA, offset1);                 // ICW2: Master PIC vector offset
	outb(PIC2_DATA, offset2);                 // ICW2: Slave PIC vector offset
	outb(PIC1_DATA, 4);                       // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	outb(PIC2_DATA, 2);                       // ICW3: tell Slave PIC its cascade identity (0000 0010)
 
	outb(PIC1_DATA, ICW4_8086);
	outb(PIC2_DATA, ICW4_8086);
 
	outb(PIC1_DATA, a1);   // restore saved masks.
	outb(PIC2_DATA, a2);
}

static void _set_all_interrupts()
{
// 32 base CPU interrupts
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
    
    // 16 PIC interrupts
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
}