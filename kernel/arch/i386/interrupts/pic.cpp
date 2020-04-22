#include <arch/i386/pic.h>

void Pic::Initialize(uint32_t base)
{
    LOG_SA("PIC: ", "Initializing... Base: %d\n", base);
    Pic::_PicRemap(base, base + 8);

    Pic::_EnableDevices(0xfc, 0xff);

    Pic::_AddToInterruptTable();
}

inline void Pic::_EnableDevices(uint8_t master_dev, uint8_t slave_dev)
{
    outb(0x21, master_dev);
    outb(0xa1, slave_dev);
    asm volatile ("sti;");
}

inline void Pic::_PicRemap(uint8_t offset1, uint8_t offset2)
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

inline void Pic::_AddToInterruptTable()
{
    Interrupts::AddToIdt(32, reinterpret_cast<uint32_t>(irq_0), 0x08, 0x8E);
    Interrupts::AddToIdt(33, reinterpret_cast<uint32_t>(irq_1), 0x08, 0x8E);
    Interrupts::AddToIdt(34, reinterpret_cast<uint32_t>(irq_2), 0x08, 0x8E);
    Interrupts::AddToIdt(35, reinterpret_cast<uint32_t>(irq_3), 0x08, 0x8E);
    Interrupts::AddToIdt(36, reinterpret_cast<uint32_t>(irq_4), 0x08, 0x8E);
    Interrupts::AddToIdt(37, reinterpret_cast<uint32_t>(irq_5), 0x08, 0x8E);
    Interrupts::AddToIdt(38, reinterpret_cast<uint32_t>(irq_6), 0x08, 0x8E);
    Interrupts::AddToIdt(39, reinterpret_cast<uint32_t>(irq_7), 0x08, 0x8E);
    Interrupts::AddToIdt(40, reinterpret_cast<uint32_t>(irq_8), 0x08, 0x8E);
    Interrupts::AddToIdt(41, reinterpret_cast<uint32_t>(irq_9), 0x08, 0x8E);
    Interrupts::AddToIdt(42, reinterpret_cast<uint32_t>(irq_10), 0x08, 0x8E);
    Interrupts::AddToIdt(43, reinterpret_cast<uint32_t>(irq_11), 0x08, 0x8E);
    Interrupts::AddToIdt(44, reinterpret_cast<uint32_t>(irq_12), 0x08, 0x8E);
    Interrupts::AddToIdt(45, reinterpret_cast<uint32_t>(irq_13), 0x08, 0x8E);
    Interrupts::AddToIdt(46, reinterpret_cast<uint32_t>(irq_14), 0x08, 0x8E);
    Interrupts::AddToIdt(47, reinterpret_cast<uint32_t>(irq_15), 0x08, 0x8E);
}