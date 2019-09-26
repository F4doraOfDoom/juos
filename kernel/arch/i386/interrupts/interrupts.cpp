#include <include/interrupts.h>

interrupts::handler interrupt_handlers[I386_INTERRUPTS] = { 0 };

void interrupts::initialize()
{
    outb(0x21,0xfc);
    outb(0xa1,0xff);
    asm volatile ("sti;");

    for(int i = 0; i < I386_INTERRUPTS; i++)
    {
        interrupt_handlers[i] = interrupts::handler{nullptr};
    }
}

void interrupts::set_handler(uint32_t interrupt_num, interrupts::handler handler)
{
    interrupt_handlers[interrupt_num]= handler;
}

void irq_handler(registers32_t registers)
{
    //printf("Got to IRQ handler with code %c", registers.int_no + 48);

    // if the interrupt came from a slave PIC, reset it
    if (PIC_IS_SLAVE(registers.int_no))
    {
        PIC_ACK_SLAVE();
    }
    else
    {
        PIC_ACK_MASTER();    
    }
    
    GO_TO_INTERRUPT(registers.int_no);
}

void isr_handler(registers32_t registers)
{
    GO_TO_INTERRUPT(registers.int_no);
}