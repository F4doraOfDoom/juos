#include <kernel/tty.h>
#include <kernel/kdef.h>
#include <kernel/interrupts.h>

#include <include/interrupts.h>
#include <string.h>

#define I386_INTERRUPTS 256

Interrupts::handler handlers[I386_INTERRUPTS] = { 0 };

void Interrupts::initialize()
{
    for(int i = 0; i < I386_INTERRUPTS; i++)
    {
        handlers[i] = Interrupts::handler{nullptr};
    }
}

void Interrupts::set_handler(uint32_t interrupt_num, Interrupts::handler handler)
{
    handlers[interrupt_num]= handler;
}

void isr_handler(Idt::registers_t registers)
{
    if (handlers[registers.int_no] != nullptr)
    {
        Tty::writestring("I am bad");
        GO_PANIC();
        handlers[registers.int_no]((void*)&registers);
    }
    else
    {
        Tty::writestring("PANIC! Got interrupt with no handler!");
        GO_PANIC();
    } 
}