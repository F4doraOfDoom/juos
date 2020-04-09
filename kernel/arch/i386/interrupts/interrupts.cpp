#include <arch/i386/interrupts.h>

constexpr struct _InterruptDescription {
    uint32_t interrupt_num;
    const char* description;
} interrupt_descriptions[] = {
    {0, "Divide by zero"},
    {1, "Single step"},
    {2, "Non-maskable"},
    {3, "Breakpoint"},
    {4, "Overflow trap"},
    {5, "Bound range exceeded"},
    {6, "Invalid opcode"},
    {7, "Co-processor not available"},
    {8, "Double fault"},
    {9, "Co-processor segment overrun"},
    {10, "Invalid TSS"},
    {11, "Segment not present"},
    {12, "Stack exception"},
    {13, "General protection exception"},
    {14, "Page fault"},
    {15, "Reserved"},
    {16, "Co-processor error"}
};

Interrupts::handler interrupt_handlers[I386_INTERRUPTS] = { 0 };

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
        char* interrupt_explanation = (char*)"Unknown";
        if (num < (sizeof(interrupt_descriptions) / sizeof(_InterruptDescription)))
        {
            interrupt_explanation = (char*)interrupt_descriptions[num].description;
        }

        LOG_SA("INTERRUPTS:", "Interrupt %d without any handler! (%s)\n", num, interrupt_explanation);
#endif
    }
}

void Interrupts::Initialize()
{
    LOG_S("INTERRUPTS: ", "Initializing...\n");
    for(int i = 0; i < I386_INTERRUPTS; i++)
    {
        interrupt_handlers[i] = Interrupts::handler{nullptr};
    }
}

void Interrupts::AddToIdt(int32_t entry, uint32_t base, uint16_t sel, uint8_t flags)
{
    Idt::EditEntry(entry, base, sel, flags);
}

void Interrupts::set_handler(uint32_t interrupt_num, Interrupts::handler handler)
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