//#include <stdio.h>
#include <kernel/tty.h>
#include <kernel/kdef.h>
#include <kernel/dtables.h>
#include <kernel/interrupts.h>
#include <kernel/timer.h>
#include <kernel/kconstants.h>
#include <kernel/kheap.h>

#include <include/dtables_structs.h>

#include <stdio.h>
#include <string.h>

using namespace kernel;

void divide_by_zero(void*);

__NO_MANGELING void kernel_main(void) {
	tty::initialize();
	gdt::initialize();
	idt::initialize();
	interrupts::initialize();

	uint32_t* str = (uint32_t*)heap::allocate(16);

	memcpy(str, "Hello world!\0", 13);

	printf("%s\n", str);

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
	printf("Divide by zero! %d", ((registers32_t*)r)->eax);
}