#include <include/pic.h>

void pic::initialize(uint32_t base)
{
    pic::_pic_remap(base, base + 8);

    pic::_enable_devices(0xfc, 0xff);

    pic::_add_to_int_table();
}

static inline void pic::_enable_devices(uint8_t master_dev, uint8_t slave_dev)
{
    outb(0x21, master_dev);
    outb(0xa1, slave_dev);
    asm volatile ("sti;");
}

static inline void pic::_pic_remap(uint8_t offset1, uint8_t offset2)
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

static inline void pic::_add_to_int_table()
{
    interrupts::add_to_idt(32, reinterpret_cast<uint32_t>(irq_0), 0x08, 0x8E);
    interrupts::add_to_idt(33, reinterpret_cast<uint32_t>(irq_1), 0x08, 0x8E);
    interrupts::add_to_idt(34, reinterpret_cast<uint32_t>(irq_2), 0x08, 0x8E);
    interrupts::add_to_idt(35, reinterpret_cast<uint32_t>(irq_3), 0x08, 0x8E);
    interrupts::add_to_idt(36, reinterpret_cast<uint32_t>(irq_4), 0x08, 0x8E);
    interrupts::add_to_idt(37, reinterpret_cast<uint32_t>(irq_5), 0x08, 0x8E);
    interrupts::add_to_idt(38, reinterpret_cast<uint32_t>(irq_6), 0x08, 0x8E);
    interrupts::add_to_idt(39, reinterpret_cast<uint32_t>(irq_7), 0x08, 0x8E);
    interrupts::add_to_idt(40, reinterpret_cast<uint32_t>(irq_8), 0x08, 0x8E);
    interrupts::add_to_idt(41, reinterpret_cast<uint32_t>(irq_9), 0x08, 0x8E);
    interrupts::add_to_idt(42, reinterpret_cast<uint32_t>(irq_10), 0x08, 0x8E);
    interrupts::add_to_idt(43, reinterpret_cast<uint32_t>(irq_11), 0x08, 0x8E);
    interrupts::add_to_idt(44, reinterpret_cast<uint32_t>(irq_12), 0x08, 0x8E);
    interrupts::add_to_idt(45, reinterpret_cast<uint32_t>(irq_13), 0x08, 0x8E);
    interrupts::add_to_idt(46, reinterpret_cast<uint32_t>(irq_14), 0x08, 0x8E);
    interrupts::add_to_idt(47, reinterpret_cast<uint32_t>(irq_15), 0x08, 0x8E);
}