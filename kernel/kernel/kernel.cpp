//#include <stdio.h>
#include <kernel/tty.h>
#include <kernel/kdef.h>
#include <kernel/dtables.h>
#include <kernel/interrupts.h>
#include <kernel/timer.h>
#include <kernel/kconstants.h>
#include <kernel/paging.h>
#include <kernel/kheap.h>
#include <kernel/klog.h>
#include <list.hpp>

#include <include/dtables_structs.h>

#include <stdio.h>
#include <string.h>

using namespace kernel;

void divide_by_zero(void*);

class Allocator
{

};

__NO_MANGELING void kernel_main(void) {
	/**
	 * The initialization order is important 
	 * 1. tty
	 * 2. (gdt/ idt)
	 * 3. interrupts
	 * 4. paging
	 * everything else... 
	 *
	 */
	tty::initialize();
	gdt::initialize();
	idt::initialize();
	interrupts::initialize();
	timer::start(K_INTERNAL_CLOCK_TICK_RATE);

	std::linked_list<int, Allocator> list;
	list.foo(1);

	paging::initialize();
	printf("Hello paing world!\n");
	//uint32_t *ptr = (uint32_t*)0xA0000000;
   	//uint32_t do_page_fault = *ptr;

	
	for (;;)
	{
		asm volatile (
			"hlt;"
		);
	}
}

void divide_by_zero(void* r)
{
	LOG_A("Divide by zero! %d", ((registers32_t*)r)->eax);
}