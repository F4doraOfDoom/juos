//#include <stdio.h>
#include <kernel/tty.h>
#include <kernel/kdef.h>
#include <kernel/dtables.h>
#include <kernel/interrupts.h>
#include <kernel/timer.h>
#include <kernel/kconstants.h>
#include <kernel/paging.h>
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
	timer::start(K_INTERNAL_CLOCK_TICK_RATE);

	printf("Hello paging world!\n");

	interrupts::set_handler(0, divide_by_zero);
	interrupts::set_handler(8, [](void*){
		GO_PANIC("got double fault!", "");
	});

	paging::initialize();
	uint32_t *ptr = (uint32_t*)0xA0000000;
   	uint32_t do_page_fault = *ptr;

	// enter infinite loop
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