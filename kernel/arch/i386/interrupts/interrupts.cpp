#include <include/interrupts.h>

interrupts::handler interrupt_handlers[I386_INTERRUPTS] = { 0 };

static void _go_to_interrupt(const registers32_t& registers)
{
    const uint16_t num = registers.int_no;

    if (interrupt_handlers[num] != nullptr) { 
        interrupt_handlers[num]((void*)&registers); 
    } 
    else { 
#ifdef K_PANIC_NO_HANDLER
        GO_PANIC("Interrupt %d without any handler!\n", num); 
#else
        LOG_SA("INTERRUPTS:", "Interrupt %d without any handler!\n", num);
#endif
    }
}

void interrupts::initialize()
{
    LOG_S("INTERRUPTS: ", "Initializing...\n");
    for(int i = 0; i < I386_INTERRUPTS; i++)
    {
        interrupt_handlers[i] = interrupts::handler{nullptr};
    }
}

void interrupts::add_to_idt(int32_t entry, uint32_t base, uint16_t sel, uint8_t flags)
{
    idt::edit_entry(entry, base, sel, flags);
}

void interrupts::set_handler(uint32_t interrupt_num, interrupts::handler handler)
{
    interrupt_handlers[interrupt_num]= handler;
}

void irq_handler(registers32_t registers)
{
    //LOG("Got to IRQ handler with code %c", registers.int_no + 48);

    // if the interrupt came from a slave PIC, reset it
    if (PIC_IS_SLAVE(registers.int_no))
    {
        PIC_ACK_SLAVE();
    }
    else
    {
        PIC_ACK_MASTER();    
    }
    
    _go_to_interrupt(registers);
}

void isr_handler(registers32_t registers)
{
    _go_to_interrupt(registers);
}