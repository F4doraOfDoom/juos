#include <include/interrupts.h>

interrupts::handler interrupt_handlers[I386_INTERRUPTS] = { 0 };

void interrupts::initialize()
{
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
    printf("Got to IRQ handler with code %c", registers.int_no + 45);

    // if the interrupt came from a slave PIC, reset it
    if (PIC_IS_SLAVE(registers.int_no))
    {
        outb(PIC_SLAVE, PIC_RESET);
    }

    // reset the master PIC
    outb(PIC_MASTER, PIC_RESET);

    GO_TO_INTERRUPT(registers.int_no);
}

void isr_handler(registers32_t registers)
{
    GO_TO_INTERRUPT(registers.int_no);
}