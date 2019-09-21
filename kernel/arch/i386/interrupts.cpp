#include <kernel/tty.h>
#include <kernel/kdef.h>
#include <kernel/interrupts.h>

#include <include/interrupts.h>

interrupt_handler handlers[256] = { 0 };

void isr_handler(Idt::registers_t registers)
{
    // Tty::writestring("Got interrupt: ");
    // Tty::putchar((char)(registers.int_no));

    if (handlers[registers.int_no])
    {
        handlers[registers.int_no]((void*)&registers);
    }
    else
    {
        Tty::writestring("PANIC! Got interrupt with no handler!");
        GO_PANIC();
    } 
}

void set_interrupt_handler(uint32_t interrupt_num, interrupt_handler handler)
{
    handlers[interrupt_num] = handler;
}