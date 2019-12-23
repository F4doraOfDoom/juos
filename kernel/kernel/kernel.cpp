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
#include <kernel/knew.hpp>
#include <kernel/kuseful.h>

#include <kernel/kstdcxx.hpp>
#include <list.hpp>

#include <include/dtables_structs.h>

#include <stdio.h>
#include <string.h>

using namespace kernel;

void divide_by_zero(void*);

struct Something
{
	Something()
	{
		memcpy(arr, "Hello new world!\n", 16);
	}

	char arr[16] = { 0 };
};

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

	// after this initialization, operators new and delete can be used
	memory_manager::initialize(
		K_HEAP_START, // beginning address
		K_HEAP_START + K_HEAP_INITIAL_SIZE, // end address
		0xCFFFF000, // max address
		false, // running in supervisor mode
		true // is read-write
	);

	timer::start(K_INTERNAL_CLOCK_TICK_RATE);

	printf("Hello timer!\n");
	timer::sleep(50);
	printf("We slept half a second!\n");

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