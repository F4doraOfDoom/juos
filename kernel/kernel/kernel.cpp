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
#include <kernel/kmm.h>

#include <kernel/kstdcxx.hpp>
#include <list.hpp>

#include <include/dtables_structs.h>

#include <stdio.h>
#include <string.h>

using namespace kernel;

void divide_by_zero(void*);

__NO_MANGELING void kernel_main(void) {
	/**
	 * The initialization order is important 
	 * 1. tty
	 * 2. (gdt/ idt)
	 * 3. interrupts
	 * 4. memory managment
	 * everything else... 
	 *
	 */
	tty::initialize();
	gdt::initialize();
	idt::initialize();
	interrupts::initialize();

	memory_manager::initialize(
		K_HEAP_START, // beginning address
		K_HEAP_START + K_HEAP_INITIAL_SIZE, // end address
		0xCFFFF000, // max address
		false, // running in supervisor mode
		true // is read-write
	);

	timer::start(K_INTERNAL_CLOCK_TICK_RATE);

	// test allocator
	// std::list<int, Less<int>, memory_manager::PrimitiveAllocator> l;
	// l.push_back(5);
	// l.push_back(6);
	// l.push_back(7);

	// for(const auto i : l)
	// {
	// 	printf("%d\n", i);
	// }

	printf("Hello paing world!\n");
	//uint32_t *ptr = (uint32_t*)0xA0000000;
   	//uint32_t do_page_fault = *ptr;

	void* a = memory_manager::malloc(16);
	void* b = memory_manager::malloc(32);
	void* c = memory_manager::malloc(32);

	printf("allocated %p\n", a);
	printf("allocated %p\n", b);
	printf("allocated %p\n", c);

	memory_manager::free(a);
	memory_manager::free(b);
	memory_manager::free(c);

	a = memory_manager::malloc(16);
	b = memory_manager::malloc(16);
	c = memory_manager::malloc(16);
	printf("allocated %p\n", a);
	printf("allocated %p\n", b);
	printf("allocated %p\n", c);

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