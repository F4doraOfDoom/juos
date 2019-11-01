//#include <stdio.h>
#include <kernel/tty.h>
#include <kernel/kdef.h>
#include <kernel/dtables.h>
#include <kernel/interrupts.h>
#include <kernel/timer.h>
#include <kernel/kconstants.h>
#include <include/dtables_structs.h>

#include <stdio.h>

void divide_by_zero(void*);

__NO_MANGELING void kernel_main(void) {
	tty::initialize();
	gdt::initialize();
	idt::initialize();
	interrupts::initialize();

	printf("Hello world!");

	timer::start(K_INTERNAL_CLOCK_TICK_RATE);

	interrupts::set_handler(0, divide_by_zero);
	
	for (;;)
	{
		asm volatile (
			"hlt;"
		);
	}
}

void divide_by_zero(void* r)
{
	//tty::writestring("STOP! YOU VIOLATED THE LAW!");
	printf("Divide by zero! %c", ((registers32_t*)r)->eax);
}