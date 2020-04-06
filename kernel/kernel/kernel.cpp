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
#include <kernel/kallocators.hpp>
#include <kernel/kuseful.h>
#include <kernel/processing.h>
#include <drivers/ata.h>
#include <drivers/ext2.h>
#include <kernel/schedualer.h>

#include <kernel/kstdcxx.hpp>
#include <list.hpp>

#include <stdio.h>
#include <string.h>

using namespace kernel;

void divide_by_zero(void*);
__NO_MANGELING void kernel_main(uint32_t);

void BREAKPOINT() {
	
}

void loop()
{
	static uint32_t counter = 0;
	while (true)
	{
		//kernel::data_structures::String str("Im looping!\n");
		printf("loop %d\n", counter++);
	}
}

void kernel_main_stage_2(void)
{
	LOG_S("KERNEL: ", "Initiating stage 2...\n");

	printf("Hello processing!\n");
}

uint32_t __stack_top = 0;

__NO_MANGELING void kernel_main(uint32_t stack_top) {
	// the beginning of the stack is now here
	__stack_top = stack_top;

	/**
	 * The initialization order is important 
	 * 1. tty
	 * 2. (gdt/ idt)
	 * 3. interrupts
	 * 4. memory managment
	 * everything else... 
	 *
	 */
	Tty::Initialize();
	Gdt::Initialize();
	Idt::Initialize();
	Interrupts::Initialize();
	// after this initialization, operators new and delete can be used
	MemoryManager::Initialize(
		K_HEAP_START, // beginning address
		K_HEAP_START + K_HEAP_INITIAL_SIZE, // end address
		0xCFFFF000, // max address
		false, // running in supervisor mode
		true // is read-write
	);

	Timer::start(K_INTERNAL_CLOCK_TICK_RATE);

	Interrupts::set_handler(1, [](auto) {});

	auto proc_scheduler = new scheduler::ProcessScheduler();
	Processing::Initialize(kernel_main_stage_2, scheduler::run_process_scheduler, proc_scheduler);
	
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

// enable pure virtual functions
__NO_MANGELING void __cxa_pure_virtual() {}