#include <kernel/tty.h>
#include <kernel/kdef.h>
#include <kernel/interrupts.h>

#include <include/interrupts.h>
#include <string.h>

#define I386_INTERRUPTS 256

interrupts::handler handlers[I386_INTERRUPTS] = { 0 };

void interrupts::initialize()
{
    for(int i = 0; i < I386_INTERRUPTS; i++)
    {
        handlers[i] = interrupts::handler{nullptr};
    }
}

void interrupts::set_handler(uint32_t interrupt_num, interrupts::handler handler)
{
    handlers[interrupt_num]= handler;
}

void isr_handler(idt::registers_t registers)
{
    if (handlers[registers.int_no] != nullptr)
    {
        handlers[registers.int_no]((void*)&registers);
    }
    else
    {
        tty::writestring("PANIC! Got interrupt with no handler!");
        GO_PANIC();
    } 
}